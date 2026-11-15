#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb framework flash verb (common across all fbb_* SDKs).

Design principle: NOTHING here is hardcoded for any specific chip. All
chip-specific values (chipName, default baud, bin_path pattern, supported
protocols) come from the SDK's own per-chip JSON at:

    build/config/target_config/<chip>/<chip>.json

Every fbb_* SDK uses this same script verbatim; only its <chip>.json files
differ. No chip-specific tables / branches in this file.

Note on the burn tool: at the time of writing, command-line burn through
BurnToolCmd has NOT been validated against real hardware for all chips.
This module wires up BurnToolCmd as a starting point because the binary
ships with the HiSpark toolchain; if a particular chip needs a different
flasher, only `_invoke_flasher()` below needs to branch on chip name.
"""

import json
import shutil
import subprocess
import sys
import time
from pathlib import Path
from typing import Any, Dict, List, Optional


# ============================================================
# JSON summary helpers (contract v1.0 §5.2)
# ============================================================

def _build_summary(
    *,
    success: bool,
    duration: float,
    target: Optional[str],
    chip: Optional[str],
    fwpkg: Optional[Path] = None,
    port: Optional[str] = None,
    baud: Optional[int] = None,
    die_id: Optional[str] = None,
    burned_sections: int = 0,
    log_path: Optional[str] = None,
    error_code: Optional[str] = None,
    error_message: Optional[str] = None,
) -> Dict[str, Any]:
    fwpkg_str = str(fwpkg) if fwpkg else None
    fwpkg_size = None
    if fwpkg and fwpkg.exists():
        try:
            fwpkg_size = fwpkg.stat().st_size
        except OSError:
            pass
    return {
        "verb": "flash",
        "schema_version": 1,
        "success": success,
        "duration_seconds": round(duration, 3),
        "target": target,
        "chip": chip,
        "fwpkg": fwpkg_str,
        "fwpkg_size_bytes": fwpkg_size,
        "port": port,
        "baud": baud,
        "die_id": die_id,
        "burned_sections": burned_sections,
        "log_path": log_path,
        "error": {"code": error_code, "message": error_message} if error_code else None,
    }


def _emit_summary(summary: Dict[str, Any], enabled: bool) -> None:
    if enabled:
        print(json.dumps(summary, ensure_ascii=False), flush=True)

# ============================================================
# Read chips list from fbb-sdk.toml
# ============================================================

def _load_sdk_meta(sdk_root: Path) -> Dict[str, Any]:
    """Return parsed fbb-sdk.toml or empty dict."""
    toml_path = sdk_root / "fbb-sdk.toml"
    if not toml_path.exists():
        return {}
    try:
        import tomllib
    except ImportError:
        try:
            import tomli as tomllib  # type: ignore
        except ImportError:
            return {}
    with open(toml_path, "rb") as f:
        return tomllib.load(f)


def _list_chips(sdk_root: Path) -> List[str]:
    meta = _load_sdk_meta(sdk_root)
    chips = meta.get("fbb-sdk", {}).get("chips", [])
    if not chips:
        # Fall back to scanning target_config/
        tc_dir = sdk_root / "build" / "config" / "target_config"
        if tc_dir.exists():
            chips = [
                d.name for d in tc_dir.iterdir()
                if d.is_dir() and (d / f"{d.name}.json").exists()
            ]
    return chips


# ============================================================
# Resolve target -> chip
# ============================================================

def _load_chip_json(sdk_root: Path, chip: str) -> Dict[str, Any]:
    p = sdk_root / "build" / "config" / "target_config" / chip / f"{chip}.json"
    if not p.exists():
        return {}
    try:
        return json.loads(p.read_text(encoding="utf-8"))
    except Exception as e:
        sys.stderr.write(f"[fbb] failed to parse {p}: {e}\n")
        return {}


def _target_belongs_to_chip(target: str, chip_json: Dict[str, Any]) -> bool:
    """Heuristic: target belongs to this chip if it appears in the chip's
    target tree. Observed structure is nested as:
        target -> {GROUP -> {<TARGET-NAME-UPPER>: ...}}
    Different SDKs may nest differently; this walker is tolerant.
    """
    target_section = chip_json.get("target")
    if not isinstance(target_section, dict):
        return False
    # Search any depth for the target name (case-insensitive)
    needle = target.upper().replace("-", "_")
    def _walk(node: Any) -> bool:
        if isinstance(node, dict):
            for k, v in node.items():
                if k.upper().replace("-", "_") == needle:
                    return True
                if _walk(v):
                    return True
        return False
    return _walk(target_section)


def _resolve_chip_for_target(sdk_root: Path, target: str) -> Optional[str]:
    """Try to map target -> chip using SDK's own JSON config.

    Strategy 1: prefix match against declared chips (cheap, works for
    `ws63-liteos-app` -> `ws63` and `bs20-standard` -> `bs20`).
    Strategy 2: scan each chip's JSON for the target name.
    """
    chips = _list_chips(sdk_root)
    if not chips:
        return None

    # Strategy 1: prefix
    target_lower = target.lower()
    for chip in chips:
        if target_lower.startswith(chip.lower()):
            return chip

    # Strategy 2: scan
    for chip in chips:
        if _target_belongs_to_chip(target, _load_chip_json(sdk_root, chip)):
            return chip

    # Last resort: if SDK has only one chip, use it
    if len(chips) == 1:
        return chips[0]
    return None


# ============================================================
# Read upload config from <chip>.json
# ============================================================

def _read_upload_config(chip_json: Dict[str, Any]) -> Dict[str, Any]:
    """Pull out the relevant upload settings, with sane fallbacks."""
    upload = chip_json.get("upload", {})
    serial_param: Dict[str, Any] = {}
    for p in upload.get("params", []):
        if p.get("name") == "serial":
            serial_param = p.get("param", {}) or {}
            break

    return {
        "default_baud": int(serial_param.get("baud") or "115200"),
        "default_port": serial_param.get("port") or "",
        "stop_bit": serial_param.get("stop_bit") or "0",
        "parity": serial_param.get("parity") or "N",
        "protocols": upload.get("protocol", ["serial"]),
    }


# ============================================================
# fwpkg path
# ============================================================

def _find_fwpkg(sdk_root: Path, chip: str, target: str, load_only: bool) -> Optional[Path]:
    """Convention: output/<chip>/fwpkg/<target>/<target>_{all,load_only}.fwpkg."""
    suffix = "_load_only" if load_only else "_all"
    candidate = sdk_root / "output" / chip / "fwpkg" / target / f"{target}{suffix}.fwpkg"
    return candidate if candidate.exists() else None


# ============================================================
# Serial port
# ============================================================

def _autodetect_port() -> Optional[str]:
    try:
        import serial.tools.list_ports
    except ImportError:
        sys.stderr.write(
            "[fbb] pyserial not available; cannot auto-detect port. Pass --port.\n"
        )
        return None
    ports = list(serial.tools.list_ports.comports())
    if len(ports) == 0:
        sys.stderr.write("[fbb] no serial ports detected.\n")
        return None
    if len(ports) == 1:
        return ports[0].device
    sys.stderr.write("[fbb] multiple serial ports found; pick one with --port:\n")
    for p in ports:
        sys.stderr.write(f"        {p.device}  ({p.description})\n")
    return None


# ============================================================
# Flasher invocation (the one place that knows about BurnToolCmd)
# ============================================================

def _invoke_flasher(
    *,
    chip_burn_name: str,
    fwpkg: Path,
    port: str,
    baud: int,
    stop_bit: str,
    parity_n_or_zero: str,
    programmer: str,
) -> int:
    """Invoke the actual burn tool. Replace this function if a different
    flasher is needed (e.g. if BurnToolCmd doesn't support some chip).
    """
    burn = shutil.which("BurnToolCmd") or shutil.which("BurnToolCmd.exe")
    if not burn:
        sys.stderr.write(
            "[fbb] BurnToolCmd not found on PATH.\n"
            "      Make sure the hs-fbb-cli env is activated "
            "(or run via `fbb flash`, which auto-activates).\n"
        )
        return 3

    # Inline-args mode (per BurnToolCmd --help):
    #   burntoolcmd.exe --burn -n <chip> -m serial <port> <baud> <stop> <parity> -f <file> -p <programmer>
    cmd = [
        burn, "--burn",
        "-n", chip_burn_name,
        "-m", "serial", port, str(baud), stop_bit, parity_n_or_zero,
        "-f", str(fwpkg),
        "-p", programmer,
    ]
    print(f"[fbb] flash: {fwpkg.name} -> {port} @ {baud}", flush=True)
    print(f"[fbb] cmd  : {' '.join(cmd)}", flush=True)
    return subprocess.call(cmd)


# ============================================================
# Entry
# ============================================================

def run_flash(args, sdk_root_str: str) -> int:
    sdk_root = Path(sdk_root_str)
    started = time.monotonic()
    json_summary_enabled = bool(getattr(args, "json_summary", False))

    def _exit(code: int, summary: Dict[str, Any]) -> int:
        _emit_summary(summary, json_summary_enabled)
        return code

    # 1. Find chip for this target
    chip = _resolve_chip_for_target(sdk_root, args.target)
    if not chip:
        sys.stderr.write(
            f"[fbb] cannot determine chip for target '{args.target}'.\n"
            f"      Known chips: {_list_chips(sdk_root)}\n"
        )
        return _exit(2, _build_summary(
            success=False, duration=time.monotonic() - started,
            target=args.target, chip=None,
            error_code="TARGET_NOT_FOUND",
            error_message=f"cannot resolve chip for target '{args.target}'",
        ))

    # 2. Load chip-specific config from SDK
    chip_json = _load_chip_json(sdk_root, chip)
    chip_burn_name = args.chip_name or chip_json.get("chipName", chip)
    upload_cfg = _read_upload_config(chip_json)

    # 3. Locate firmware artifact
    fwpkg = _find_fwpkg(sdk_root, chip, args.target, args.load_only)
    if not fwpkg:
        suffix = "_load_only" if args.load_only else "_all"
        sys.stderr.write(
            f"[fbb] firmware not found for target '{args.target}' (chip {chip}).\n"
            f"      expected: output/{chip}/fwpkg/{args.target}/{args.target}{suffix}.fwpkg\n"
            f"      run `fbb build {args.target}` first.\n"
        )
        return _exit(1, _build_summary(
            success=False, duration=time.monotonic() - started,
            target=args.target, chip=chip,
            error_code="FWPKG_NOT_FOUND",
            error_message=f"firmware not found; run `fbb build {args.target}` first",
        ))

    # 4. Resolve port + baud (CLI overrides JSON defaults)
    port = args.port or upload_cfg["default_port"] or _autodetect_port()
    if not port:
        return _exit(3, _build_summary(
            success=False, duration=time.monotonic() - started,
            target=args.target, chip=chip, fwpkg=fwpkg,
            error_code="PORT_NOT_FOUND",
            error_message="no serial port specified and auto-detect found none",
        ))
    baud = args.baud or upload_cfg["default_baud"]
    programmer = args.programmer or "HiSpark-Trace"

    # 5. Invoke flasher
    rc = _invoke_flasher(
        chip_burn_name=chip_burn_name,
        fwpkg=fwpkg,
        port=port,
        baud=baud,
        stop_bit=upload_cfg["stop_bit"],
        parity_n_or_zero="0" if upload_cfg["parity"].upper() == "N" else "1",
        programmer=programmer,
    )

    # 6. Build summary based on flasher outcome
    if rc == 0:
        return _exit(0, _build_summary(
            success=True, duration=time.monotonic() - started,
            target=args.target, chip=chip, fwpkg=fwpkg,
            port=port, baud=baud,
            # die_id / burned_sections / log_path are TODO once we parse the
            # underlying tool's log; left as None for now.
        ))
    # Distinguish two common failure code ranges from the flasher
    if rc == 3:
        return _exit(3, _build_summary(
            success=False, duration=time.monotonic() - started,
            target=args.target, chip=chip, fwpkg=fwpkg,
            port=port, baud=baud,
            error_code="FLASHER_NOT_FOUND",
            error_message="BurnToolCmd not found on PATH",
        ))
    return _exit(rc if rc != 0 else 1, _build_summary(
        success=False, duration=time.monotonic() - started,
        target=args.target, chip=chip, fwpkg=fwpkg,
        port=port, baud=baud,
        error_code="FLASH_FAILED",
        error_message=f"flasher returned non-zero exit code {rc}",
    ))
