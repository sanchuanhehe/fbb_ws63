#!/usr/bin/env python3
"""
WebSocket回显服务端
支持异步和同步两种实现方式
"""

import asyncio
import json
import logging
import socket
import threading
import time
from datetime import datetime
from typing import Set, Any

try:
    import websockets
    from websockets.server import WebSocketServerProtocol
    ASYNC_AVAILABLE = True
except ImportError:
    ASYNC_AVAILABLE = False
    print("警告: websockets库未安装，异步服务器不可用")
    print("请运行: pip install websockets")

try:
    from websocket_server import WebsocketServer
    SYNC_AVAILABLE = True
except ImportError:
    SYNC_AVAILABLE = False
    print("警告: websocket-server库未安装，同步服务器不可用")
    print("请运行: pip install websocket-server")

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)


class AsyncWebSocketEchoServer:
    """异步WebSocket回显服务器"""
    
    def __init__(self, host: str = "0.0.0.0", port: int = 8765):
        self.host = host
        self.port = port
        self.clients: Set[WebSocketServerProtocol] = set()
        self.message_count = 0
        
    async def register_client(self, websocket: WebSocketServerProtocol):
        """注册新客户端"""
        self.clients.add(websocket)
        client_info = f"{websocket.remote_address[0]}:{websocket.remote_address[1]}"
        logger.info(f"客户端已连接: {client_info} (总连接数: {len(self.clients)})")
        
    async def unregister_client(self, websocket: WebSocketServerProtocol):
        """注销客户端"""
        self.clients.discard(websocket)
        client_info = f"{websocket.remote_address[0]}:{websocket.remote_address[1]}"
        logger.info(f"客户端已断开: {client_info} (总连接数: {len(self.clients)})")
        
    async def handle_message(self, websocket: WebSocketServerProtocol, message: str):
        """处理接收到的消息"""
        self.message_count += 1
        client_info = f"{websocket.remote_address[0]}:{websocket.remote_address[1]}"
        
        logger.info(f"收到来自 {client_info} 的消息 #{self.message_count}: {message}")
        
        # 创建回显响应
        echo_response = {
            "type": "echo",
            "original_message": message,
            "timestamp": datetime.now().isoformat(),
            "client": client_info,
            "message_id": self.message_count,
            "server_info": f"Python AsyncWebSocket Echo Server"
        }
        
        # 发送回显消息
        response_str = json.dumps(echo_response, ensure_ascii=False, indent=2)
        await websocket.send(response_str)
        logger.info(f"已回显消息给 {client_info}")
        
    async def handle_client(self, websocket: WebSocketServerProtocol):
        """处理客户端连接"""
        await self.register_client(websocket)
        try:
            async for message in websocket:
                await self.handle_message(websocket, message)
        except websockets.exceptions.ConnectionClosed:
            logger.info("客户端连接已关闭")
        except Exception as e:
            logger.error(f"处理客户端时发生错误: {e}")
        finally:
            await self.unregister_client(websocket)
            
    async def start_server(self):
        """启动异步服务器"""
        logger.info(f"启动异步WebSocket回显服务器，监听 {self.host}:{self.port}")
        
        async with websockets.serve(self.handle_client, self.host, self.port):
            logger.info("异步WebSocket服务器已启动，按Ctrl+C停止")
            # 保持服务器运行
            await asyncio.Future()  # 永远等待
            

class SyncWebSocketEchoServer:
    """同步WebSocket回显服务器"""
    
    def __init__(self, host: str = "0.0.0.0", port: int = 8766):
        self.host = host
        self.port = port
        self.message_count = 0
        self.server = None
        
    def new_client(self, client, server):
        """新客户端连接回调"""
        client_info = f"{client['address'][0]}:{client['address'][1]}"
        logger.info(f"客户端已连接: {client_info} (ID: {client['id']})")
        
        # 发送欢迎消息
        welcome_msg = {
            "type": "welcome",
            "message": "欢迎连接到WebSocket回显服务器！",
            "timestamp": datetime.now().isoformat(),
            "client_id": client['id'],
            "server_info": "Python SyncWebSocket Echo Server"
        }
        server.send_message(client, json.dumps(welcome_msg, ensure_ascii=False))
        
    def client_left(self, client, server):
        """客户端断开连接回调"""
        client_info = f"{client['address'][0]}:{client['address'][1]}"
        logger.info(f"客户端已断开: {client_info} (ID: {client['id']})")
        
    def message_received(self, client, server, message):
        """接收到消息回调"""
        self.message_count += 1
        client_info = f"{client['address'][0]}:{client['address'][1]}"
        
        logger.info(f"收到来自 {client_info} 的消息 #{self.message_count}: {message}")
        
        # 创建回显响应
        echo_response = {
            "type": "echo",
            "original_message": message,
            "timestamp": datetime.now().isoformat(),
            "client": client_info,
            "client_id": client['id'],
            "message_id": self.message_count,
            "server_info": "Python SyncWebSocket Echo Server"
        }
        
        # 发送回显消息
        response_str = json.dumps(echo_response, ensure_ascii=False, indent=2)
        server.send_message(client, response_str)
        logger.info(f"已回显消息给 {client_info}")
        
    def start_server(self):
        """启动同步服务器"""
        logger.info(f"启动同步WebSocket回显服务器，监听 {self.host}:{self.port}")
        
        self.server = WebsocketServer(host=self.host, port=self.port)
        self.server.set_fn_new_client(self.new_client)
        self.server.set_fn_client_left(self.client_left)
        self.server.set_fn_message_received(self.message_received)
        
        logger.info("同步WebSocket服务器已启动，按Ctrl+C停止")
        self.server.run_forever()


def get_local_ip():
    """获取本机IP地址"""
    try:
        # 创建一个UDP连接来获取本机IP
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return "127.0.0.1"


def main():
    """主函数"""
    print("=" * 60)
    print("WebSocket回显服务器")
    print("=" * 60)
    
    local_ip = get_local_ip()
    print(f"本机IP地址: {local_ip}")
    
    print("\n可用的服务器类型:")
    if ASYNC_AVAILABLE:
        print("  1. 异步WebSocket服务器 (推荐) - 端口 8765")
    if SYNC_AVAILABLE:
        print("  2. 同步WebSocket服务器 - 端口 8766")
    if not ASYNC_AVAILABLE and not SYNC_AVAILABLE:
        print("  错误: 没有可用的WebSocket库!")
        print("  请安装依赖: pip install websockets websocket-server")
        return
        
    print("  3. 同时启动两个服务器")
    print("  0. 退出")
    
    while True:
        try:
            choice = input("\n请选择服务器类型 (1-3): ").strip()
            
            if choice == "0":
                print("退出程序")
                break
            elif choice == "1" and ASYNC_AVAILABLE:
                server = AsyncWebSocketEchoServer()
                print(f"\n启动异步服务器...")
                print(f"WebSocket URL: ws://{local_ip}:8765")
                print(f"本地测试URL: ws://localhost:8765")
                asyncio.run(server.start_server())
                break
            elif choice == "2" and SYNC_AVAILABLE:
                server = SyncWebSocketEchoServer()
                print(f"\n启动同步服务器...")
                print(f"WebSocket URL: ws://{local_ip}:8766")
                print(f"本地测试URL: ws://localhost:8766")
                server.start_server()
                break
            elif choice == "3":
                if not ASYNC_AVAILABLE or not SYNC_AVAILABLE:
                    print("错误: 需要安装所有依赖才能同时启动两个服务器")
                    continue
                    
                print(f"\n启动两个服务器...")
                print(f"异步服务器: ws://{local_ip}:8765")
                print(f"同步服务器: ws://{local_ip}:8766")
                
                # 在单独的线程中启动同步服务器
                sync_server = SyncWebSocketEchoServer()
                sync_thread = threading.Thread(target=sync_server.start_server, daemon=True)
                sync_thread.start()
                
                # 等待一秒确保同步服务器启动
                time.sleep(1)
                
                # 在主线程中启动异步服务器
                async_server = AsyncWebSocketEchoServer()
                asyncio.run(async_server.start_server())
                break
            else:
                print("无效选择，请重新输入")
        except KeyboardInterrupt:
            print("\n\n收到中断信号，正在关闭服务器...")
            break
        except Exception as e:
            logger.error(f"服务器错误: {e}")
            print(f"服务器错误: {e}")


if __name__ == "__main__":
    main()