#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb framework `create-project` / `create-component` verbs.

Scaffolds a new project or component by cloning one of the built-in
templates under <sdk>/tools/templates/. Chip-agnostic: the templates
may reference chip-specific config (e.g. APP_TARGETS for components), but
the scaffold logic is generic.

Contract v1.0:
  - exit 0 success
  - exit 1 general failure (template missing, copy failed)
  - exit 2 usage error (bad args)
  - exit 4 destination not empty / not a directory
"""

import json
import re
import shutil
import sys
import time
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple


# Built-in scaffolding templates live under this path, relative to the SDK
# src/ root. Grouped with other developer tooling assets under tools/.
_TEMPLATES_SUBDIR = Path("tools") / "templates"


# ============================================================
# Example & template discovery
# ============================================================

def _load_example_roots(sdk_root: Path) -> List[Path]:
    """Read [examples].roots from fbb-sdk.toml. Returns absolute paths."""
    toml_path = sdk_root / "fbb-sdk.toml"
    if not toml_path.exists():
        return []
    try:
        import tomllib
    except ImportError:
        try:
            import tomli as tomllib  # type: ignore
        except ImportError:
            return []
    try:
        with open(toml_path, "rb") as f:
            meta = tomllib.load(f)
    except Exception:
        return []
    roots = meta.get("examples", {}).get("roots", []) or []
    return [sdk_root / r for r in roots if isinstance(r, str)]


def _resolve_template_or_example(
    name: str, sdk_root: Path
) -> Tuple[Optional[Path], Optional[str]]:
    """Search for `name` first under tools/templates/ then each example root.
    Returns (path, kind) where kind is 'template' or 'example'."""
    tpl = sdk_root / _TEMPLATES_SUBDIR / name
    if tpl.is_dir():
        return tpl, "template"
    for root in _load_example_roots(sdk_root):
        candidate = root / name
        if candidate.is_dir() and (candidate / "CMakeLists.txt").exists():
            return candidate, "example"
    return None, None


def _list_examples(sdk_root: Path) -> Dict[str, List[str]]:
    """Return {root_relative_path: [example_name, ...]} for every declared root."""
    out: Dict[str, List[str]] = {}
    for root in _load_example_roots(sdk_root):
        if not root.is_dir():
            continue
        names = sorted(
            d.name for d in root.iterdir()
            if d.is_dir() and (d / "CMakeLists.txt").exists()
        )
        rel = str(root.relative_to(sdk_root)).replace("\\", "/")
        out[rel] = names
    return out


# ============================================================
# JSON summary (contract v1.0 §5.2)
# ============================================================

def _summary(
    *,
    verb: str,
    success: bool,
    duration: float,
    name: str,
    path: Optional[Path],
    template: Optional[str],
    files_created: int = 0,
    error_code: Optional[str] = None,
    error_message: Optional[str] = None,
) -> Dict[str, Any]:
    return {
        "verb": verb,
        "schema_version": 1,
        "success": success,
        "duration_seconds": round(duration, 3),
        "name": name,
        "path": str(path) if path else None,
        "template": template,
        "files_created": files_created,
        "error": {"code": error_code, "message": error_message} if error_code else None,
    }


def _emit(summary: Dict[str, Any], enabled: bool) -> None:
    if enabled:
        print(json.dumps(summary, ensure_ascii=False), flush=True)


# ============================================================
# Filesystem helpers
# ============================================================

def _resolve_dest(name: str, path_override: Optional[str]) -> Path:
    """Compute destination path: <path_override or cwd>/<name>."""
    base = Path(path_override).resolve() if path_override else Path.cwd()
    return base / name


def _ensure_empty_dir(dest: Path) -> Optional[str]:
    """Create dest if missing. Return error message if dest is not usable."""
    if dest.exists():
        if not dest.is_dir():
            return f"destination exists and is not a directory: {dest}"
        if any(dest.iterdir()):
            return f"destination is not empty: {dest}"
    else:
        dest.mkdir(parents=True, exist_ok=True)
    return None


def _copy_template(src_template: Path, dest: Path) -> int:
    """Copy template tree into dest. Return file count."""
    count = 0
    for item in src_template.iterdir():
        src = src_template / item.name
        dst = dest / item.name
        if src.is_file():
            shutil.copy2(src, dst)
            count += 1
        else:
            shutil.copytree(src, dst)
            count += sum(1 for _ in dst.rglob("*") if _.is_file())
    return count


# Match identifier-safe placeholders so we don't accidentally munge unrelated text.
_NAME_RE = re.compile(r"\bsample_project\b|\bsample_component\b")


def _rename_token_in_files(dest: Path, old_token: str, new_name: str) -> None:
    """Replace occurrences of `old_token` with `new_name` in text files at
    the top level of dest (CMakeLists.txt, README.md). We intentionally do
    NOT recurse — deep replacements have surprised users in other tools."""
    for fname in ("CMakeLists.txt", "README.md"):
        f = dest / fname
        if not f.exists():
            continue
        try:
            text = f.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue
        new_text = text.replace(old_token, new_name)
        if new_text != text:
            f.write_text(new_text, encoding="utf-8")


# ============================================================
# Core scaffolder (shared by both verbs)
# ============================================================

def _scaffold(
    *,
    verb: str,
    source_dir: Path,
    source_label: str,
    rename_token: Optional[str],
    name: str,
    path_override: Optional[str],
    json_summary: bool,
) -> int:
    """Generic scaffolder. `source_dir` is the already-resolved template or
    example directory; `source_label` is what to put in the JSON summary's
    `template` field (informational)."""
    started = time.monotonic()

    def _done(code: int, summary: Dict[str, Any]) -> int:
        _emit(summary, json_summary)
        return code

    # 1. Validate name (basic identifier sanity — avoid path traversal)
    if not name or "/" in name or "\\" in name or name in (".", ".."):
        sys.stderr.write(f"[fbb] invalid name: {name!r}\n")
        return _done(2, _summary(
            verb=verb, success=False, duration=time.monotonic() - started,
            name=name, path=None, template=source_label,
            error_code="INVALID_NAME",
            error_message="name must not contain path separators or be . / ..",
        ))

    # 2. Resolve + validate destination
    dest = _resolve_dest(name, path_override)
    err = _ensure_empty_dir(dest)
    if err:
        sys.stderr.write(f"[fbb] {err}\n")
        return _done(4, _summary(
            verb=verb, success=False, duration=time.monotonic() - started,
            name=name, path=dest, template=source_label,
            error_code="DEST_NOT_EMPTY", error_message=err,
        ))

    # 3. Copy + (optionally) rename a project/component token
    try:
        files = _copy_template(source_dir, dest)
        if rename_token:
            _rename_token_in_files(dest, rename_token, name)
    except OSError as e:
        sys.stderr.write(f"[fbb] copy failed: {e}\n")
        return _done(1, _summary(
            verb=verb, success=False, duration=time.monotonic() - started,
            name=name, path=dest, template=source_label,
            error_code="COPY_FAILED", error_message=str(e),
        ))

    print(f"[fbb] {verb}: created '{name}' at {dest} ({files} file(s))", flush=True)
    return _done(0, _summary(
        verb=verb, success=True, duration=time.monotonic() - started,
        name=name, path=dest, template=source_label,
        files_created=files,
    ))


# ============================================================
# Verb entries (called by fbb_dispatch)
# ============================================================

def run_create_project(args, sdk_root_str: str) -> int:
    """Scaffold a new project.

    --template <name>: resolves first under <sdk>/tools/templates/, then under
    each declared example root. Default is the built-in sample_project template.
    """
    sdk_root = Path(sdk_root_str)
    json_summary = bool(getattr(args, "json_summary", False))
    template_name = getattr(args, "template", None) or "sample_project"
    source_dir, kind = _resolve_template_or_example(template_name, sdk_root)
    if source_dir is None:
        sys.stderr.write(
            f"[fbb] template/example {template_name!r} not found under "
            f"{sdk_root}/tools/templates/ or any [examples].roots in fbb-sdk.toml.\n"
        )
        if json_summary:
            print(json.dumps(_summary(
                verb="create-project", success=False, duration=0.0,
                name=args.name, path=None, template=template_name,
                error_code="TEMPLATE_NOT_FOUND",
                error_message=f"unknown template/example: {template_name}",
            ), ensure_ascii=False), flush=True)
        return 1

    # Only rename when starting from the built-in sample_project template;
    # cloning a real example preserves its own naming.
    rename_token = "sample_project" if kind == "template" and template_name == "sample_project" else None

    return _scaffold(
        verb="create-project",
        source_dir=source_dir,
        source_label=template_name,
        rename_token=rename_token,
        name=args.name,
        path_override=getattr(args, "path", None),
        json_summary=json_summary,
    )


def run_create_component(args, sdk_root_str: str) -> int:
    """Scaffold a new component from <sdk>/tools/templates/sample_component/."""
    sdk_root = Path(sdk_root_str)
    template_dir = sdk_root / _TEMPLATES_SUBDIR / "sample_component"
    return _scaffold(
        verb="create-component",
        source_dir=template_dir,
        source_label="sample_component",
        rename_token="sample_component",
        name=args.name,
        path_override=getattr(args, "path", None),
        json_summary=bool(getattr(args, "json_summary", False)),
    )


def run_create_project_from_example(args, sdk_root_str: str) -> int:
    """Clone an existing example from one of the declared `[examples].roots`.

    Unlike `create-project --template`, this verb refuses to fall back to the
    sample template — it requires a real example.
    """
    sdk_root = Path(sdk_root_str)
    json_summary = bool(getattr(args, "json_summary", False))
    example = args.example
    source_dir, kind = _resolve_template_or_example(example, sdk_root)
    if source_dir is None or kind != "example":
        # Build a useful error message that includes available examples
        available = _list_examples(sdk_root)
        flat = [f"{root}/{name}" for root, names in available.items() for name in names]
        sys.stderr.write(
            f"[fbb] example {example!r} not found. "
            f"Run `fbb list-examples` to see {len(flat)} available examples.\n"
        )
        if json_summary:
            print(json.dumps(_summary(
                verb="create-project-from-example", success=False, duration=0.0,
                name=getattr(args, "name", None) or example, path=None, template=example,
                error_code="EXAMPLE_NOT_FOUND",
                error_message=f"unknown example: {example}",
            ), ensure_ascii=False), flush=True)
        return 1

    # If user didn't pass --name, use the example's name as the project name.
    project_name = getattr(args, "name", None) or example

    return _scaffold(
        verb="create-project-from-example",
        source_dir=source_dir,
        source_label=example,
        rename_token=None,  # examples have their own naming; don't munge
        name=project_name,
        path_override=getattr(args, "path", None),
        json_summary=json_summary,
    )
