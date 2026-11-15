#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb framework `list-examples` verb.

Enumerates clonable examples by reading `[examples].roots` from
fbb-sdk.toml. Each entry under a root that contains a CMakeLists.txt
is treated as an example.
"""

import json
import sys
import time
from pathlib import Path
from typing import Any, Dict


def _build_listing(sdk_root: Path) -> Dict[str, Any]:
    # Reuse fbb_create's discovery — it already loads the toml and filters
    # by CMakeLists.txt presence.
    from fbb_create import _list_examples
    by_root = _list_examples(sdk_root)
    flat = [f"{root}/{name}" for root, names in by_root.items() for name in names]
    return {
        "verb": "list-examples",
        "schema_version": 1,
        "by_root": by_root,
        "all_examples": sorted(flat),
        "total": len(flat),
    }


def _print_text(listing: Dict[str, Any]) -> None:
    print("Available examples:")
    print("=" * 50)
    if not listing["by_root"]:
        print("  (none — fbb-sdk.toml has no [examples].roots)")
        return
    for root, names in listing["by_root"].items():
        print(f"\n[{root}]")
        if names:
            for n in names:
                print(f"  {n}")
        else:
            print("  (empty)")
    print(f"\nTotal: {listing['total']} example(s)")


def run_list_examples(args, sdk_root_str: str) -> int:
    started = time.monotonic()
    json_summary = bool(getattr(args, "json_summary", False))
    json_only = bool(getattr(args, "json", False))

    listing = _build_listing(Path(sdk_root_str))
    listing["success"] = True
    listing["duration_seconds"] = round(time.monotonic() - started, 3)

    if json_only:
        print(json.dumps(listing, ensure_ascii=False, indent=2))
    else:
        _print_text(listing)
        if json_summary:
            print(json.dumps(listing, ensure_ascii=False), flush=True)
    return 0
