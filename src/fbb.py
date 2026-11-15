#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026.
# All rights reserved.
"""
fbb 框架 SDK 现代化命令行入口（与 build.py 并存）。

本脚本在所有 fbb_* SDK 间通用——chip 专属数据全部从
`build/config/target_config/<chip>/<chip>.json` 读取；该 SDK 自身的元数据
（chip 列表等）从同目录 `fbb-sdk.toml` 读取。把本文件原样复制到任何
fbb_* SDK 的 src/ 下即可。

新用户推荐通过 hs-fbb-cli 间接调用：
    fbb build <target>
    fbb flash <target>
    fbb monitor

也可直接调用本脚本：
    python fbb.py build <target>              # 例如 ws63-liteos-app
    python fbb.py flash <target> --port COMx
    python fbb.py monitor --port COMx

老的 build.py 依然可用，行为不变；不需要立即迁移。
"""

import os
import sys


def _bootstrap_path():
    here = os.path.dirname(os.path.realpath(__file__))
    sys.path.insert(0, os.path.join(here, "build", "config"))
    sys.path.insert(0, os.path.join(here, "build", "script"))
    sys.dont_write_bytecode = True
    return here


SDK_ROOT = _bootstrap_path()

from fbb_dispatch import main  # noqa: E402

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:], sdk_root=SDK_ROOT))
