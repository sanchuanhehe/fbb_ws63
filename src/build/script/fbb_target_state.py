#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb framework `set-target` / `get-target` verbs + per-project state file.

State file is `.fbb-target` placed at the *project root* — which we define
as `FBB_BUILD_ROOT_PATH` if set, else the current working directory. This
intentionally avoids ~/.hbf_target (a per-user file that collides across
different SDK checkouts and projects).

Contract v1.0:
  - exit 0 success
  - exit 1 invalid target / not found
  - exit 2 usage error
  - exit 3 state file missing (only for get-target)
"""

import json
import os
import sys
import time
from pathlib import Path
from typing import Any, Dict, List, Optional


STATE_FILENAME = ".fbb-target"


# ============================================================
# State file location
# ============================================================

def _project_root() -> Path:
    """Where the .fbb-target file lives.

    Priority: FBB_BUILD_ROOT_PATH (out-of-tree project) > cwd.
    """
    override = os.environ.get("FBB_BUILD_ROOT_PATH")
    if override:
        return Path(override).resolve()
    return Path.cwd().resolve()


def state_file_path() -> Path:
    return _project_root() / STATE_FILENAME


def read_saved_target() -> Optional[str]:
    """Return the saved target, or None if no state file exists."""
    f = state_file_path()
    if not f.exists():
        return None
    try:
        t = f.read_text(encoding="utf-8").strip()
        return t or None
    except OSError:
        return None


# ============================================================
# JSON summary
# ============================================================

def _summary(
    *,
    verb: str,
    success: bool,
    duration: float,
    target: Optional[str],
    state_file: Path,
    available_targets_count: Optional[int] = None,
    error_code: Optional[str] = None,
    error_message: Optional[str] = None,
) -> Dict[str, Any]:
    return {
        "verb": verb,
        "schema_version": 1,
        "success": success,
        "duration_seconds": round(duration, 3),
        "target": target,
        "state_file": str(state_file),
        "available_targets_count": available_targets_count,
        "error": {"code": error_code, "message": error_message} if error_code else None,
    }


def _emit(summary: Dict[str, Any], enabled: bool) -> None:
    if enabled:
        print(json.dumps(summary, ensure_ascii=False), flush=True)


# ============================================================
# Target validation (lazy import - enviroment is heavy)
# ============================================================

def _known_targets() -> List[str]:
    """Read enviroment.all_target / all_group. Imports lazily because
    enviroment.py walks every chip's target_config at import time."""
    from enviroment import all_target, all_group
    return list(all_target.keys()) + list(all_group.keys())


def _target_exists(target: str) -> bool:
    return target in _known_targets()


# ============================================================
# Verb entries
# ============================================================

def run_set_target(args, _sdk_root_str: str) -> int:
    started = time.monotonic()
    json_summary = bool(getattr(args, "json_summary", False))
    target = args.target
    state_file = state_file_path()

    def _done(code: int, **kw) -> int:
        _emit(_summary(verb="set-target", duration=time.monotonic() - started,
                       state_file=state_file, **kw), json_summary)
        return code

    # Validate against known targets
    try:
        known = _known_targets()
    except Exception as e:
        sys.stderr.write(f"[fbb] failed to load target list: {e}\n")
        return _done(1, success=False, target=target,
                     error_code="ENVIRONMENT_LOAD_FAILED", error_message=str(e))

    if target not in known:
        sys.stderr.write(
            f"[fbb] target {target!r} not found. "
            f"Run `fbb list-targets` to see {len(known)} available targets.\n"
        )
        return _done(1, success=False, target=target,
                     available_targets_count=len(known),
                     error_code="TARGET_NOT_FOUND",
                     error_message=f"unknown target: {target}")

    try:
        state_file.parent.mkdir(parents=True, exist_ok=True)
        state_file.write_text(target, encoding="utf-8")
    except OSError as e:
        sys.stderr.write(f"[fbb] failed to write state file: {e}\n")
        return _done(1, success=False, target=target,
                     available_targets_count=len(known),
                     error_code="STATE_WRITE_FAILED", error_message=str(e))

    print(f"[fbb] target set: {target}  ({state_file})", flush=True)
    return _done(0, success=True, target=target, available_targets_count=len(known))


def run_get_target(args, _sdk_root_str: str) -> int:
    started = time.monotonic()
    json_summary = bool(getattr(args, "json_summary", False))
    state_file = state_file_path()
    target = read_saved_target()

    def _done(code: int, **kw) -> int:
        _emit(_summary(verb="get-target", duration=time.monotonic() - started,
                       state_file=state_file, **kw), json_summary)
        return code

    if not target:
        sys.stderr.write(
            f"[fbb] no target saved at {state_file}. "
            f"Run `fbb set-target <name>` first.\n"
        )
        return _done(3, success=False, target=None,
                     error_code="STATE_NOT_FOUND",
                     error_message=f"no state file at {state_file}")

    print(target, flush=True)
    return _done(0, success=True, target=target)
