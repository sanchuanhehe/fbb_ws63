#!/bin/bash
# 激活虚拟环境
source .venv/bin/activate
export LC_ALL=C.UTF-8
export PATH="/sdk/tools/bin/compiler/riscv/cc_riscv32_musl_b090/cc_riscv32_musl_fp/bin:${PATH}"
echo "✓ 开发环境已激活"
echo "使用 'deactivate' 命令退出虚拟环境"
