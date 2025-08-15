#!/bin/bash
# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

# 激活虚拟环境
source ${SCRIPT_DIR}/.venv/bin/activate
export LC_ALL=C.UTF-8
export PATH="${SCRIPT_DIR}/tools/bin/compiler/riscv/cc_riscv32_musl_b090/cc_riscv32_musl_fp/bin:${PATH}"
echo "✓ 开发环境已激活"
echo "使用 'deactivate' 命令退出虚拟环境"
