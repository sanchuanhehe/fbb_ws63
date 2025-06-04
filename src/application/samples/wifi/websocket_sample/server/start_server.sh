#!/bin/bash

# WebSocket回显服务器启动脚本

echo "WebSocket回显服务器启动脚本"
echo "=============================="

# 检查Python是否安装
if ! command -v python3 &> /dev/null; then
    echo "错误: Python3未安装"
    exit 1
fi

# 创建虚拟环境（如果不存在）
if [ ! -d "venv" ]; then
    echo "创建Python虚拟环境..."
    python3 -m venv venv
fi

# 激活虚拟环境
echo "激活虚拟环境..."
source venv/bin/activate

# 检查依赖是否安装
echo "检查依赖..."
python3 -c "import websockets" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "websockets库未安装，正在安装..."
    pip install websockets
fi

python3 -c "import websocket_server" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "websocket-server库未安装，正在安装..."
    pip install websocket-server
fi

echo "启动WebSocket回显服务器..."
python3 server.py
