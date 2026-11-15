#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb framework - verb dispatcher (common across all fbb_* SDKs).

Translates modern verb-style invocations (`fbb.py build <target> ...`) into
the underlying SDK operations. For `build`, this means rebuilding the legacy
flag-style argv that CMakeBuilder expects; for `flash` / `monitor`, this
delegates to dedicated modules.

This file is chip-agnostic: chip-specific data is in <chip>.json under
build/config/target_config/, SDK identity is in fbb-sdk.toml. Adding a new
verb here is purely additive — does NOT touch build.py.
"""

import argparse
import sys
from typing import List


def _make_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="fbb",
        description="fbb_* SDK build / flash / monitor entry. "
                    "(Forward-compatible companion to build.py; both coexist.)",
    )
    sub = p.add_subparsers(dest="verb")

    # ---- build ----
    pb = sub.add_parser(
        "build",
        help="Compile a target (incremental). "
             "Wraps build/script/cmake_builder.py:CMakeBuilder.",
    )
    pb.add_argument("target", nargs="?",
                    help="Target name (chip-specific, e.g. ws63-liteos-app / bs20-standard). "
                         "Omit to let build.py print the list.")
    pb.add_argument("-j", "--jobs", type=int,
                    help="Parallel build jobs (default: cpu count).")
    pb.add_argument("-D", "--define", action="append", default=[],
                    metavar="KEY[=VAL]",
                    help="Add a compile-time macro (repeatable). "
                         "Equivalent to legacy `-def=KEY,KEY2=VAL`.")
    pb.add_argument("--component", action="append", default=[],
                    metavar="NAME",
                    help="Build only the named component (repeatable). "
                         "Equivalent to legacy `-component=NAME`.")
    pb.add_argument("--release", action="store_true",
                    help="Release-mode disassembly (faster).")
    pb.add_argument("--debug", action="store_true",
                    help="Debug-mode disassembly (more info, slower).")
    pb.add_argument("--dump-flags", action="store_true",
                    help="Dump compile flags. Equivalent to legacy `-dump`.")
    pb.add_argument("--no-hso", action="store_true",
                    help="Skip HSO database update. Equivalent to legacy `-nhso`.")
    pb.add_argument("--out-libs", metavar="PATH",
                    help="Pack all .a files into a single archive at PATH "
                         "instead of linking the elf. Equivalent to legacy `-out_libs=PATH`.")
    pb.add_argument("--make", action="store_true",
                    help="Use Unix make instead of ninja. (default: ninja)")

    # ---- flash ----
    pf = sub.add_parser("flash", help="Burn target firmware to a serial port.")
    pf.add_argument("target", help="Target to flash (chip-specific, e.g. ws63-liteos-app)")
    pf.add_argument("--port", help="Serial port (auto-detect if omitted).")
    pf.add_argument("--baud", type=int,
                    help="Baud rate (default: from <chip>.json upload.params.serial.baud).")
    pf.add_argument("--programmer", default=None,
                    help="Programmer name (default: HiSpark-Trace).")
    pf.add_argument("--chip-name", default=None,
                    help="BurnTool chip name override (default: <chip>.json chipName).")
    pf.add_argument("--load-only", action="store_true",
                    help="Use *_load_only.fwpkg instead of *_all.fwpkg.")
    pf.add_argument("--json-summary", action="store_true",
                    help="On exit, print a one-line JSON summary to stdout "
                         "(see contract v1.0).")

    # ---- monitor ----
    pm = sub.add_parser("monitor", help="Open a serial console on the device.")
    pm.add_argument("--port", help="Serial port (auto-detect if omitted).")
    pm.add_argument("--baud", type=int, default=None,
                    help="Baud rate (default: from <chip>.json monitor.default_baud).")
    pm.add_argument("--chip", default=None,
                    help="Chip name (default: single-chip SDK auto-picks).")
    pm.add_argument("--until", default=None, metavar="REGEX",
                    help="Exit 0 on first match; exit 4 on timeout without match.")
    pm.add_argument("--timeout", type=float, default=None, metavar="SECONDS",
                    help="Wall-clock seconds limit. Required with --until "
                         "for HIL flows.")
    pm.add_argument("--reset", action="store_true",
                    help="Before reading, send the reset command from "
                         "<chip>.json monitor.reset_command (e.g. AT+RST).")
    pm.add_argument("--json-summary", action="store_true",
                    help="On exit, print a one-line JSON summary to stdout "
                         "(see contract v1.0).")
    pm.add_argument("--log", metavar="FILE",
                    help="Tee captured bytes to FILE in addition to stdout.")

    # ---- create-project ----
    pcp = sub.add_parser(
        "create-project",
        help="Scaffold a new project from <sdk>/tools/templates/sample_project/.",
    )
    pcp.add_argument("name", help="Project name (directory + cmake project token).")
    pcp.add_argument("-p", "--path", default=None,
                     help="Parent directory (default: current dir).")
    pcp.add_argument("--template", default=None,
                     help="Template name under <sdk>/tools/templates/ (default: sample_project).")
    pcp.add_argument("--json-summary", action="store_true",
                     help="On exit, print a one-line JSON summary to stdout "
                          "(see contract v1.0).")

    # ---- create-component ----
    pcc = sub.add_parser(
        "create-component",
        help="Scaffold a new component from <sdk>/tools/templates/sample_component/.",
    )
    pcc.add_argument("name", help="Component name (directory + COMPONENT_NAME).")
    pcc.add_argument("-p", "--path", default=None,
                     help="Parent directory (default: current dir).")
    pcc.add_argument("--json-summary", action="store_true",
                     help="On exit, print a one-line JSON summary to stdout "
                          "(see contract v1.0).")

    # ---- create-project-from-example ----
    pce = sub.add_parser(
        "create-project-from-example",
        help="Clone an example listed in fbb-sdk.toml [examples].roots.",
    )
    pce.add_argument("example",
                     help="Example name (see `fbb list-examples`).")
    pce.add_argument("--name", default=None,
                     help="Project directory name (default: same as example).")
    pce.add_argument("-p", "--path", default=None,
                     help="Parent directory (default: current dir).")
    pce.add_argument("--json-summary", action="store_true",
                     help="On exit, print a one-line JSON summary to stdout.")

    # ---- list-examples ----
    ple = sub.add_parser(
        "list-examples",
        help="Enumerate clonable examples from fbb-sdk.toml [examples].roots.",
    )
    ple.add_argument("--json", action="store_true",
                     help="Emit structured JSON to stdout instead of text.")
    ple.add_argument("--json-summary", action="store_true",
                     help="In addition to text output, print a one-line JSON "
                          "summary as the final line of stdout.")

    # ---- set-target ----
    pst = sub.add_parser(
        "set-target",
        help="Remember a default target in <project>/.fbb-target. "
             "Use `fbb get-target` to read it back.",
    )
    pst.add_argument("target", help="Target name (must exist; see `fbb list-targets`).")
    pst.add_argument("--json-summary", action="store_true",
                     help="On exit, print a one-line JSON summary to stdout.")

    # ---- get-target ----
    pgt = sub.add_parser(
        "get-target",
        help="Print the target saved by `fbb set-target`.",
    )
    pgt.add_argument("--json-summary", action="store_true",
                     help="On exit, print a one-line JSON summary to stdout.")

    # ---- list-targets ----
    plt = sub.add_parser(
        "list-targets",
        help="Enumerate buildable targets and groups, grouped by chip.",
    )
    plt.add_argument("--json", action="store_true",
                     help="Emit structured JSON to stdout instead of text.")
    plt.add_argument("--json-summary", action="store_true",
                     help="In addition to text output, print a one-line JSON "
                          "summary as the final line of stdout.")

    return p


# ============================================================
# build verb
# ============================================================

def _cmd_build(args, _sdk_root: str) -> int:
    """Translate new args back to legacy argv and call CMakeBuilder."""
    legacy = ["build.py"]
    if args.target:
        legacy.append(args.target)
    if args.jobs:
        legacy.append(f"-j{args.jobs}")
    if args.define:
        legacy.append("-def=" + ",".join(args.define))
    if args.component:
        legacy.append("-component=" + ",".join(args.component))
    if args.release:
        legacy.append("-release")
    if args.debug:
        legacy.append("-debug")
    if args.dump_flags:
        legacy.append("-dump")
    if args.no_hso:
        legacy.append("-nhso")
    if args.out_libs:
        legacy.append(f"-out_libs={args.out_libs}")
    # Default to ninja for new fbb users (legacy default was Make for build.py;
    # ninja is faster and what hs-fbb-cli ships in the toolchain).
    if not args.make:
        legacy.append("-ninja")

    # Import here so `fbb.py --help / list / etc.` doesn't pay the cmake_builder
    # import cost (it pulls in many SDK modules).
    from cmake_builder import CMakeBuilder

    print(f"[fbb] build: legacy argv = {legacy}", flush=True)
    builder = CMakeBuilder(legacy)
    builder.build()
    return 0


# ============================================================
# flash verb (delegates to fbb_flash)
# ============================================================

def _cmd_flash(args, sdk_root: str) -> int:
    from fbb_flash import run_flash
    return run_flash(args, sdk_root)


# ============================================================
# monitor verb (delegates to fbb_monitor)
# ============================================================

def _cmd_monitor(args, sdk_root: str) -> int:
    from fbb_monitor import run_monitor
    return run_monitor(args, sdk_root)


# ============================================================
# create-project / create-component (delegate to fbb_create)
# ============================================================

def _cmd_create_project(args, sdk_root: str) -> int:
    from fbb_create import run_create_project
    return run_create_project(args, sdk_root)


def _cmd_create_component(args, sdk_root: str) -> int:
    from fbb_create import run_create_component
    return run_create_component(args, sdk_root)


def _cmd_create_project_from_example(args, sdk_root: str) -> int:
    from fbb_create import run_create_project_from_example
    return run_create_project_from_example(args, sdk_root)


# ============================================================
# list-examples (delegate to fbb_list_examples)
# ============================================================

def _cmd_list_examples(args, sdk_root: str) -> int:
    from fbb_list_examples import run_list_examples
    return run_list_examples(args, sdk_root)


# ============================================================
# set-target / get-target (delegate to fbb_target_state)
# ============================================================

def _cmd_set_target(args, sdk_root: str) -> int:
    from fbb_target_state import run_set_target
    return run_set_target(args, sdk_root)


def _cmd_get_target(args, sdk_root: str) -> int:
    from fbb_target_state import run_get_target
    return run_get_target(args, sdk_root)


# ============================================================
# list-targets (delegate to fbb_list_targets)
# ============================================================

def _cmd_list_targets(args, sdk_root: str) -> int:
    from fbb_list_targets import run_list_targets
    return run_list_targets(args, sdk_root)


# ============================================================
# entry
# ============================================================

_VERB_TABLE = {
    "build":                        _cmd_build,
    "flash":                        _cmd_flash,
    "monitor":                      _cmd_monitor,
    "create-project":               _cmd_create_project,
    "create-component":             _cmd_create_component,
    "create-project-from-example":  _cmd_create_project_from_example,
    "set-target":                   _cmd_set_target,
    "get-target":                   _cmd_get_target,
    "list-targets":                 _cmd_list_targets,
    "list-examples":                _cmd_list_examples,
}


def main(argv: List[str], sdk_root: str) -> int:
    parser = _make_parser()
    args = parser.parse_args(argv)

    handler = _VERB_TABLE.get(args.verb)
    if handler:
        return handler(args, sdk_root)

    parser.print_help()
    return 0 if args.verb is None else 2
