#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.

"""
Lint: enforce ROOT_DIR over CMAKE_SOURCE_DIR for SDK-internal paths.

CMAKE_SOURCE_DIR points at cmake's source directory, which in out-of-tree
builds is the user's project, not the SDK. SDK code that needs to refer
to SDK paths must use ROOT_DIR — it is set to the SDK root in both
in-tree and out-of-tree modes.

Run from SDK src/ root. Exit codes:
  0  clean
  1  violations found
  2  invocation error (not in SDK src root)

Cross-platform (Linux / macOS / Windows). Path comparisons use POSIX
slashes throughout so the allowlist works identically everywhere.
"""

import re
import sys
from pathlib import Path

PATTERN = re.compile(r'\$\{CMAKE_SOURCE_DIR\}')

# Files allowed to reference CMAKE_SOURCE_DIR, with a reason for the
# next maintainer. Keys are POSIX-style paths relative to SDK src/.
ALLOWLIST = {
    # Root in-tree entry: derives ROOT_DIR from CMAKE_SOURCE_DIR
    # (in-tree, the two are equal).
    "CMakeLists.txt":
        "root in-tree entry derives ROOT_DIR from CMAKE_SOURCE_DIR",

    # Out-of-tree public entry: CMAKE_SOURCE_DIR here legitimately
    # points at the user's project (intentional).
    "build/cmake/project.cmake":
        "OOT entry; CMAKE_SOURCE_DIR = user project by design",

    # build_core's OOT branch adds the project's main/ and components/
    # from CMAKE_SOURCE_DIR (= user project in OOT mode).
    "build/cmake/build_core.cmake":
        "OOT epilogue extension references project dir by design",

    # LiteOS in-source-build guard (compares source vs binary dirs).
    "kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/build/cmake/CMakeLists.txt":
        "in-source-build guard (legitimate cmake_source vs cmake_binary check)",
}

# Path prefixes skipped entirely (templates, build outputs, vendored trees).
SKIP_PREFIXES = (
    "tools/templates/",
    "output/",
    "interim_binary/",
)


def is_skipped(rel: str) -> bool:
    return any(rel.startswith(p) for p in SKIP_PREFIXES)


def main() -> int:
    sdk_root = Path.cwd()
    if not (sdk_root / "build.py").exists():
        sys.stderr.write(
            "[lint] error: must be run from SDK src/ root "
            f"(no build.py at {sdk_root})\n"
        )
        return 2

    targets = sorted(set(
        list(sdk_root.rglob("*.cmake")) + list(sdk_root.rglob("CMakeLists.txt"))
    ))

    violations = []
    scanned = 0
    skipped = 0
    for f in targets:
        rel = f.relative_to(sdk_root).as_posix()
        if rel in ALLOWLIST or is_skipped(rel):
            skipped += 1
            continue
        scanned += 1
        try:
            content = f.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        for lineno, raw in enumerate(content.splitlines(), 1):
            # Strip cmake comments to avoid false positives in docs/warnings.
            line = re.sub(r'#.*$', '', raw)
            if PATTERN.search(line):
                violations.append((rel, lineno, raw.strip()))

    if violations:
        print(f"[lint] {len(violations)} forbidden CMAKE_SOURCE_DIR use(s):\n")
        for rel, lineno, line in violations:
            print(f"  {rel}:{lineno}")
            print(f"    {line}")
        print()
        print("SDK-internal cmake code must use ${ROOT_DIR} for SDK paths.")
        print("CMAKE_SOURCE_DIR points at the user's project in out-of-tree builds,")
        print("so it can silently misresolve paths and produce 'header not found'")
        print("errors that only show up in customer projects.")
        print()
        print("If your use is legitimate (e.g. parsing user-project files), add")
        print(f"the file path to ALLOWLIST in {Path(__file__).name} with a")
        print("one-line reason for the next maintainer.")
        return 1

    print(
        f"[lint] OK  scanned={scanned}  skipped={skipped}  "
        f"allowlisted={len(ALLOWLIST)}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
