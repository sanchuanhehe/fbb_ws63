# WebSocket回显服务器使用说明

## 概述

这是一个Python实现的WebSocket回显服务器，支持异步和同步两种实现方式。服务器会将客户端发送的消息原样回显，并添加时间戳、客户端信息等元数据。

## 功能特性

- **异步WebSocket服务器** (推荐): 基于`websockets`库，性能更好，支持大量并发连接
- **同步WebSocket服务器**: 基于`websocket-server`库，实现简单，适合学习
- **消息回显**: 将客户端消息原样返回，附加时间戳和客户端信息
- **客户端管理**: 跟踪连接的客户端数量和状态
- **日志记录**: 详细的连接和消息日志
- **多服务器**: 可同时启动两个服务器在不同端口

## 安装依赖

```bash
# 安装所有依赖
pip install -r requirements.txt

# 或者单独安装
pip install websockets websocket-server
```

## 快速启动

### 方法1: 使用启动脚本
```bash
./start_server.sh
```

### 方法2: 直接运行
```bash
python3 server.py
```

## 服务器选项

启动后会看到以下选项：

1. **异步WebSocket服务器** (推荐) - 端口 8765
   - 高性能，支持大量并发连接
   - WebSocket URL: `ws://localhost:8765`

2. **同步WebSocket服务器** - 端口 8766
   - 简单实现，适合学习和调试
   - WebSocket URL: `ws://localhost:8766`

3. **同时启动两个服务器**
   - 异步服务器: `ws://localhost:8765`
   - 同步服务器: `ws://localhost:8766`

## 测试连接

### 使用浏览器测试

在浏览器控制台中运行以下JavaScript代码：

```javascript
// 连接到异步服务器
const ws = new WebSocket('ws://localhost:8765');

ws.onopen = function(event) {
    console.log('已连接到WebSocket服务器');
    ws.send('Hello WebSocket!');
};

ws.onmessage = function(event) {
    console.log('收到回显:', JSON.parse(event.data));
};

ws.onclose = function(event) {
    console.log('连接已关闭');
};

ws.onerror = function(error) {
    console.log('WebSocket错误:', error);
};
```

### 使用命令行工具测试

```bash
# 使用wscat (需要先安装: npm install -g wscat)
wscat -c ws://localhost:8765

# 使用websocat (需要先安装websocat)
websocat ws://localhost:8765
```

## 消息格式

### 发送消息
客户端可以发送任意文本消息。

### 接收消息格式
服务器返回的回显消息格式如下：

```json
{
  "type": "echo",
  "original_message": "你发送的原始消息",
  "timestamp": "2025-05-24T10:30:45.123456",
  "client": "192.168.1.100:12345",
  "message_id": 1,
  "server_info": "Python AsyncWebSocket Echo Server"
}
```

### 欢迎消息 (仅同步服务器)
同步服务器在客户端连接时会发送欢迎消息：

```json
{
  "type": "welcome",
  "message": "欢迎连接到WebSocket回显服务器！",
  "timestamp": "2025-05-24T10:30:45.123456",
  "client_id": 1,
  "server_info": "Python SyncWebSocket Echo Server"
}
```

## 日志输出

服务器会输出详细的日志信息：

```
2025-05-24 10:30:45,123 - __main__ - INFO - 客户端已连接: 127.0.0.1:12345 (总连接数: 1)
2025-05-24 10:30:47,456 - __main__ - INFO - 收到来自 127.0.0.1:12345 的消息 #1: Hello WebSocket!
2025-05-24 10:30:47,457 - __main__ - INFO - 已回显消息给 127.0.0.1:12345
2025-05-24 10:30:50,789 - __main__ - INFO - 客户端已断开: 127.0.0.1:12345 (总连接数: 0)
```

## 停止服务器

按 `Ctrl+C` 停止服务器。

## 网络配置

- 服务器默认监听 `0.0.0.0`，表示接受来自任何网络接口的连接
- 异步服务器默认端口: 8765
- 同步服务器默认端口: 8766
- 可以在代码中修改主机和端口设置

## 故障排除

### 端口被占用
如果遇到端口被占用的错误，可以：
1. 修改代码中的端口号
2. 或者杀死占用端口的进程：
   ```bash
   # 查找占用端口的进程
   lsof -i :8765
   # 杀死进程
   kill -9 <PID>
   ```

### 依赖安装失败
如果pip安装失败，可以尝试：
```bash
# 升级pip
pip install --upgrade pip

# 使用国内镜像源
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple websockets websocket-server
```

## 嵌入式设备测试

这个服务器特别适合用来测试嵌入式设备（如WS63）的WebSocket客户端功能：

1. 在开发机上启动这个Python服务器
2. 确保嵌入式设备和开发机在同一网络
3. 在嵌入式设备的WebSocket客户端中连接到开发机的IP地址和端口
4. 发送测试消息，验证回显功能是否正常
