#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb framework `list-targets` verb.

Enumerates buildable targets and target groups by reading the SDK's own
`enviroment` module (which loads `target_config/<chip>/config.py` at import
time). This is more reliable than parsing `build.py`'s error output — we
read the authoritative source.

Contract v1.0:
  - exit 0 success
  - exit 1 environment load failed
"""

import json
import sys
import time
from typing import Any, Dict, List, Optional


# ============================================================
# Output shape
# ============================================================

def _build_listing() -> Dict[str, Any]:
    """Group targets+groups by chip. Lazy import: enviroment scans every
    chip's target_config on import."""
    from enviroment import all_target, all_group, chip_group_target

    by_chip: Dict[str, Dict[str, List[str]]] = {}
    for chip, names in chip_group_target.items():
        targets = sorted(n for n in names if n in all_target)
        groups = sorted(n for n in names if n in all_group)
        by_chip[chip] = {"targets": targets, "groups": groups}

    return {
        "verb": "list-targets",
        "schema_version": 1,
        "by_chip": by_chip,
        "all_targets": sorted(all_target.keys()),
        "all_groups": sorted(all_group.keys()),
        "total_targets": len(all_target),
        "total_groups": len(all_group),
    }


def _print_text(listing: Dict[str, Any]) -> None:
    print("Available targets:")
    print("=" * 50)
    for chip, items in listing["by_chip"].items():
        if items["targets"]:
            print(f"\n[{chip}] individual targets:")
            for t in items["targets"]:
                print(f"  {t}")
        if items["groups"]:
            print(f"\n[{chip}] target groups:")
            for g in items["groups"]:
                print(f"  {g}")
    print(f"\nTotal: {listing['total_targets']} targets, "
          f"{listing['total_groups']} groups across {len(listing['by_chip'])} chip(s)")


# ============================================================
# Entry
# ============================================================

def run_list_targets(args, _sdk_root_str: str) -> int:
    started = time.monotonic()
    json_summary = bool(getattr(args, "json_summary", False))
    json_only = bool(getattr(args, "json", False))

    try:
        listing = _build_listing()
    except Exception as e:
        sys.stderr.write(f"[fbb] failed to enumerate targets: {e}\n")
        err = {
            "verb": "list-targets", "schema_version": 1,
            "success": False, "duration_seconds": round(time.monotonic() - started, 3),
            "error": {"code": "ENUMERATION_FAILED", "message": str(e)},
        }
        if json_summary or json_only:
            print(json.dumps(err, ensure_ascii=False), flush=True)
        return 1

    listing["success"] = True
    listing["duration_seconds"] = round(time.monotonic() - started, 3)

    if json_only:
        print(json.dumps(listing, ensure_ascii=False, indent=2))
    else:
        _print_text(listing)
        if json_summary:
            # When both human + machine output requested: text first, then
            # a final single-line JSON (contract convention).
            print(json.dumps(listing, ensure_ascii=False), flush=True)

    return 0
