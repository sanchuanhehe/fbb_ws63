#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Build Update Pkg File
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# ============================================================================

import os
import sys
import argparse
import shutil
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))
scripts = ['build_ws63_sle_mesh_standard_update.py', 'build_ws63_sle_mesh_sample_update.py']

for script in scripts:
    script_path = os.path.join(script_dir, script)
    try:
        subprocess.run([sys.executable, script_path], check=True)
        print(f"success: {script}")
    except subprocess.CalledProcessError as e:
        print(f"error: {script} by : {e}")
    except Exception as e:
        print(f"other error ({script}): {e}")
