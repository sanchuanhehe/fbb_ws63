#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb framework monitor verb (common across all fbb_* SDKs).

Serial console with HIL-friendly extensions:
  --until <regex>           exit 0 on first match, exit 4 on timeout
  --timeout N               wall-clock seconds limit
  --reset                   send reset command from <chip>.json before reading
  --json-summary            single-line JSON on exit (see contract v1.0)
  --log FILE                tee captured bytes to file

Chip-specific defaults (baud, reset method) come from
  build/config/target_config/<chip>/<chip>.json -> "monitor" section.
If you flashed a build that reconfigures UART to a non-default baud,
pass --baud explicitly. (No auto-fallback; we don't guess.)

Schema of --json-summary output (one line, last line of stdout):
{
  "verb": "monitor",
  "schema_version": 1,
  "success": <bool>,
  "duration_seconds": <float>,
  "exit_reason": "matched" | "timeout" | "user_interrupt" | "io_error",
  "matched": <bool>,
  "match_pattern": <string|null>,
  "match_text": <string|null>,
  "bytes_read": <int>,
  "port": <string|null>,
  "baud": <int|null>,
  "log_path": <string|null>,
  "error": {"code": "...", "message": "..."} | null
}
"""

import json
import os
import re
import sys
import time
from pathlib import Path
from typing import Any, Dict, List, Optional


# ============================================================
# Chip metadata
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


def _list_chips(sdk_root: Path) -> List[str]:
    """Pull chip list from fbb-sdk.toml (chips=[...]), fallback to scanning target_config/."""
    toml_path = sdk_root / "fbb-sdk.toml"
    if toml_path.exists():
        try:
            import tomllib
        except ImportError:
            try:
                import tomli as tomllib  # type: ignore
            except ImportError:
                tomllib = None  # type: ignore
        if tomllib is not None:
            try:
                with open(toml_path, "rb") as f:
                    meta = tomllib.load(f)
                chips = meta.get("fbb-sdk", {}).get("chips", [])
                if chips:
                    return list(chips)
            except Exception:
                pass
    tc_dir = sdk_root / "build" / "config" / "target_config"
    if tc_dir.exists():
        return [
            d.name for d in tc_dir.iterdir()
            if d.is_dir() and (d / f"{d.name}.json").exists()
        ]
    return []


def _pick_chip(sdk_root: Path, chip_hint: Optional[str]) -> Optional[str]:
    """If user specified --chip, use it. Otherwise: single-chip SDK auto-picks."""
    if chip_hint:
        return chip_hint
    chips = _list_chips(sdk_root)
    if len(chips) == 1:
        return chips[0]
    if len(chips) == 0:
        return None
    sys.stderr.write(
        f"[fbb] multi-chip SDK; please specify --chip <name>. Available: {chips}\n"
    )
    return None


def _load_monitor_meta(sdk_root: Path, chip: str) -> Dict[str, Any]:
    """Read <chip>.json -> monitor section, with sane defaults."""
    chip_json = _load_chip_json(sdk_root, chip)
    monitor = chip_json.get("monitor", {})
    return {
        "default_baud": int(monitor.get("default_baud", 115200)),
        "reset_method": monitor.get("reset_method", "none"),
        "reset_command": monitor.get("reset_command", ""),
    }


# ============================================================
# Serial port auto-detect
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
    sys.stderr.write("[fbb] multiple serial ports found; specify with --port:\n")
    for p in ports:
        sys.stderr.write(f"        {p.device}  ({p.description})\n")
    return None


# ============================================================
# Result envelope
# ============================================================

def _build_summary(
    *,
    success: bool,
    duration: float,
    exit_reason: str,
    matched: bool = False,
    match_pattern: Optional[str] = None,
    match_text: Optional[str] = None,
    bytes_read: int = 0,
    port: Optional[str] = None,
    baud: Optional[int] = None,
    log_path: Optional[str] = None,
    error_code: Optional[str] = None,
    error_message: Optional[str] = None,
) -> Dict[str, Any]:
    return {
        "verb": "monitor",
        "schema_version": 1,
        "success": success,
        "duration_seconds": round(duration, 3),
        "exit_reason": exit_reason,
        "matched": matched,
        "match_pattern": match_pattern,
        "match_text": match_text,
        "bytes_read": bytes_read,
        "port": port,
        "baud": baud,
        "log_path": log_path,
        "error": {"code": error_code, "message": error_message} if error_code else None,
    }


def _emit_summary(summary: Dict[str, Any], enabled: bool) -> None:
    if enabled:
        print(json.dumps(summary, ensure_ascii=False), flush=True)


# ============================================================
# Reset
# ============================================================

def _do_reset(ser, reset_method: str, reset_command: str) -> None:
    """Send a reset to the device. Currently supports `at_rst` and `none`."""
    if reset_method == "at_rst" and reset_command:
        try:
            ser.write(reset_command.encode("utf-8"))
            ser.flush()
            print(f"[fbb] sent reset: {reset_command!r}", file=sys.stderr, flush=True)
            time.sleep(0.3)
        except Exception as e:
            sys.stderr.write(f"[fbb] reset failed: {e}\n")
    elif reset_method == "none":
        pass
    else:
        sys.stderr.write(
            f"[fbb] unsupported reset_method '{reset_method}'; skipping reset.\n"
        )


# ============================================================
# Capture loop
# ============================================================

def _capture(
    serial_mod,
    port: str,
    baud: int,
    timeout: Optional[float],
    until_re: Optional[re.Pattern],
    log_fp,
    reset_method: str,
    reset_command: str,
    do_reset: bool,
):
    """Read bytes from port until: (a) until_re matches, (b) timeout expires,
    (c) Ctrl+C, (d) I/O error.

    Returns a 6-tuple:
        (exit_reason, matched, match_text, bytes_read, err_code, err_msg)
    err_code and err_msg are None unless exit_reason == "io_error".
    """
    bytes_read = 0
    matched = False
    match_text: Optional[str] = None
    buf = bytearray()  # rolling buffer for until_re matching

    deadline = time.monotonic() + timeout if timeout else None
    try:
        ser = serial_mod.Serial(port, baud, timeout=0.2)
    except serial_mod.SerialException as e:
        return ("io_error", False, None, 0, f"PORT_OPEN_FAILED", str(e))

    print(
        f"[fbb] monitor: {port} @ {baud}"
        + (f"  until /{until_re.pattern}/" if until_re else "")
        + (f"  timeout {timeout}s" if timeout else "  (Ctrl+C to exit)"),
        file=sys.stderr, flush=True,
    )

    try:
        if do_reset:
            _do_reset(ser, reset_method, reset_command)

        while True:
            if deadline and time.monotonic() >= deadline:
                return ("timeout", False, None, bytes_read, None, None)

            chunk = ser.read(4096)
            if chunk:
                sys.stdout.buffer.write(chunk)
                sys.stdout.buffer.flush()
                if log_fp:
                    log_fp.write(chunk)
                    log_fp.flush()
                bytes_read += len(chunk)
                buf.extend(chunk)
                # Keep buffer bounded: last 64KB is enough for any reasonable regex
                if len(buf) > 65536:
                    buf = buf[-65536:]

                if until_re is not None:
                    try:
                        text = buf.decode("utf-8", errors="replace")
                    except Exception:
                        text = ""
                    m = until_re.search(text)
                    if m:
                        matched = True
                        # Capture some context around the match
                        match_text = text[max(0, m.start() - 40): m.end() + 40]
                        return ("matched", True, match_text, bytes_read, None, None)
    except KeyboardInterrupt:
        print("\n[fbb] monitor exit (Ctrl+C)", file=sys.stderr, flush=True)
        return ("user_interrupt", False, None, bytes_read, None, None)
    except serial_mod.SerialException as e:
        return ("io_error", False, None, bytes_read, "IO_ERROR", str(e))
    finally:
        try:
            ser.close()
        except Exception:
            pass


# ============================================================
# Entry
# ============================================================

def run_monitor(args, sdk_root_str: Optional[str] = None) -> int:
    sdk_root = Path(sdk_root_str) if sdk_root_str else Path(__file__).resolve().parents[2]

    try:
        import serial as serial_mod
    except ImportError:
        sys.stderr.write(
            "[fbb] pyserial is required for `fbb monitor` but is not installed.\n"
            "      Re-run `fbb setup --force` to refresh the build venv.\n"
        )
        return 3

    # Compile --until regex first (cheap, catches usage errors before opening port)
    until_re: Optional[re.Pattern] = None
    until_pat: Optional[str] = getattr(args, "until", None)
    if until_pat:
        try:
            until_re = re.compile(until_pat)
        except re.error as e:
            sys.stderr.write(f"[fbb] invalid --until regex: {e}\n")
            summary = _build_summary(
                success=False, duration=0.0, exit_reason="io_error",
                error_code="INVALID_USAGE", error_message=f"bad regex: {e}",
            )
            _emit_summary(summary, getattr(args, "json_summary", False))
            return 2

    # Chip metadata for defaults
    chip = _pick_chip(sdk_root, getattr(args, "chip", None))
    meta = _load_monitor_meta(sdk_root, chip) if chip else {
        "default_baud": 115200, "reset_method": "none", "reset_command": "",
    }

    # Resolve port
    port = args.port or _autodetect_port()
    if not port:
        summary = _build_summary(
            success=False, duration=0.0, exit_reason="io_error",
            error_code="PORT_NOT_FOUND",
            error_message="no serial port specified and auto-detect found none",
        )
        _emit_summary(summary, getattr(args, "json_summary", False))
        return 3

    # Resolve baud
    baud = args.baud or meta["default_baud"]

    # Log file
    log_fp = None
    log_path: Optional[str] = None
    if getattr(args, "log", None):
        log_path = str(Path(args.log).resolve())
        try:
            log_fp = open(log_path, "ab")
        except OSError as e:
            sys.stderr.write(f"[fbb] cannot open log file {log_path}: {e}\n")
            summary = _build_summary(
                success=False, duration=0.0, exit_reason="io_error",
                error_code="IO_ERROR", error_message=str(e), port=port,
            )
            _emit_summary(summary, getattr(args, "json_summary", False))
            return 1

    timeout: Optional[float] = float(args.timeout) if getattr(args, "timeout", None) else None

    # Single capture at the resolved baud — no auto-fallback (pass --baud explicitly
    # if your firmware reconfigured UART to a non-default rate).
    started = time.monotonic()
    last_exit_reason, last_matched, last_match_text, total_bytes, last_err_code, last_err_msg = _capture(
        serial_mod, port, baud,
        timeout=timeout,
        until_re=until_re,
        log_fp=log_fp,
        reset_method=meta["reset_method"],
        reset_command=meta["reset_command"],
        do_reset=bool(getattr(args, "reset", False)),
    )

    if log_fp:
        log_fp.close()

    used_baud = baud

    duration = time.monotonic() - started
    success = last_matched if until_re else (last_exit_reason == "user_interrupt")

    summary = _build_summary(
        success=success,
        duration=duration,
        exit_reason=last_exit_reason,
        matched=last_matched,
        match_pattern=until_pat,
        match_text=last_match_text,
        bytes_read=total_bytes,
        port=port,
        baud=used_baud,
        log_path=log_path,
        error_code=last_err_code,
        error_message=last_err_msg,
    )
    _emit_summary(summary, getattr(args, "json_summary", False))

    # Exit code per contract §4
    if last_exit_reason == "matched":
        return 0
    if last_exit_reason == "user_interrupt":
        return 0
    if last_exit_reason == "timeout":
        return 4 if until_re else 0  # only "fail" if user asked to match something
    if last_exit_reason == "io_error":
        return 3 if last_err_code == "PORT_OPEN_FAILED" else 1
    return 1
