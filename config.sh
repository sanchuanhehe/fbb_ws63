#!/bin/bash

set -e  # 出错时立即退出

# 设置非交互模式以避免时间区选择等提示
export DEBIAN_FRONTEND=noninteractive

# 定义安装Python包的函数
install_python_package() {
    local package=$1
    local package_name=$(echo $package | cut -d'>' -f1 | cut -d'=' -f1)
    
    echo "安装 $package_name..."
    for i in {1..3}; do
        echo "尝试第 $i 次安装 $package_name..."
        if pip install "$package" --timeout=120; then
            echo "✓ $package_name 安装成功"
            return 0
        else
            echo "⚠️  $package_name 安装失败，重试中..."
            if [ $i -eq 3 ]; then
                echo "❌ $package_name 安装失败，请手动安装"
                return 1
            fi
            sleep 2
        fi
    done
}

# 检查并安装系统依赖
check_and_install_package() {
    local package=$1
    if ! dpkg -l | grep -q "^ii  $package "; then
        echo "安装缺失的包: $package"
        sudo apt-get update
        sudo apt-get install -y "$package"
    else
        echo "✓ $package 已安装"
    fi
}

echo "==== 检查和安装系统依赖 ===="

# 检查必要的系统包
packages=(
    "cmake"
    "python3"
    "python3-pip"
    "python3-setuptools"
    "python3-venv"
    "python3-full"
    "wget"
    "build-essential"
    "sudo"
    "curl"
    "git"
    "bash"
    "ninja-build"
    "unzip"
    "doxygen"
    "clang-format"
    "clangd-19"
)

for package in "${packages[@]}"; do
    check_and_install_package "$package"
done

echo "==== 配置shell ===="
# 配置shell为bash
if [ -f /usr/share/debconf/confmodule ]; then
    echo "dash dash/sh boolean false" | sudo debconf-set-selections
    sudo dpkg-reconfigure -p critical dash
fi

echo "==== 设置Python虚拟环境 ===="
VENV_DIR=".venv"
if [ ! -d "$VENV_DIR" ]; then
    echo "创建Python虚拟环境: $VENV_DIR"
    python3 -m venv $VENV_DIR
    echo "✓ 虚拟环境已创建"
else
    echo "✓ 虚拟环境已存在"
fi

# 激活虚拟环境
source $VENV_DIR/bin/activate
echo "✓ 虚拟环境已激活"

echo "==== 配置pip镜像源 ===="
# 创建pip配置目录
mkdir -p ~/.pip

# 配置pip使用清华镜像源
cat > ~/.pip/pip.conf << EOF
[global]
index-url = https://pypi.tuna.tsinghua.edu.cn/simple/
trusted-host = pypi.tuna.tsinghua.edu.cn
timeout = 120
retries = 3
EOF

echo "✓ 已配置pip使用清华镜像源"

# 升级pip (带重试机制)
echo "升级pip..."
for i in {1..3}; do
    echo "尝试第 $i 次升级pip..."
    if pip install --upgrade pip --timeout=120; then
        echo "✓ pip升级成功"
        break
    else
        echo "⚠️  pip升级失败，重试中..."
        if [ $i -eq 3 ]; then
            echo "⚠️  pip升级失败，使用当前版本继续"
        fi
        sleep 2
    fi
done

echo "==== 安装Python依赖包 ====="

# 定义安装Python包的函数

# 安装其他Python依赖包，版本大于2.21
install_python_package "pycparser>=2.21"

# 安装PYYAML
install_python_package "pyyaml"

echo "==== 配置clangd ===="

# 检查clangd-19是否已安装
if command -v clangd-19 &> /dev/null; then
    echo "✓ clangd-19 已安装"
    
    # 创建或更新clangd符号链接
    if [ -L "/usr/local/bin/clangd" ] || [ -f "/usr/local/bin/clangd" ]; then
        echo "移除现有的 clangd 符号链接/文件"
        sudo rm -f /usr/local/bin/clangd
    fi
    
    # 创建指向clangd-19的符号链接
    sudo ln -sf $(which clangd-19) /usr/local/bin/clangd
    echo "✓ 已将 clangd 设置为 clangd-19"
    
    # 验证设置
    if command -v clangd &> /dev/null; then
        echo "✓ clangd 验证成功，版本: $(clangd --version | head -1)"
    else
        echo "⚠️  clangd 设置可能有问题，请检查 PATH 设置"
    fi
else
    echo "⚠️  clangd-19 未安装，请检查系统包安装是否成功"
    echo "   可手动安装: sudo apt install clangd-19"
fi

echo "==== 设置环境变量 ====="

# 设置环境变量
export LC_ALL=C.UTF-8

# 添加环境变量 PATH
export PATH="/sdk/tools/bin/compiler/riscv/cc_riscv32_musl_b090/cc_riscv32_musl_fp/bin:${PATH}"

echo "==== 环境检查总结 ===="
echo "✓ 系统依赖已安装"
echo "✓ Python虚拟环境已创建: .venv"
echo "✓ Python依赖包已安装"
echo "✓ 环境变量已设置"
echo ""
echo "使用方法："
echo "1. 每次开发前运行: source /path to /fbb_ws63/src/activate_env.sh"
echo "2. 或手动激活: source .venv/bin/activate"
echo ""
echo "环境初始化完成！"