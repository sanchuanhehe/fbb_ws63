# Wi-Fi STA模式连接时序图分析

## 时序图

```mermaid
sequenceDiagram
    participant App as 应用层
    participant Task as WebSocket任务
    participant WiFi as Wi-Fi服务
    participant State as 状态机
    participant Cb as 回调函数
    participant DHCP as DHCP服务
    
    Note over App: 应用启动
    App->>Task: app_run(websocket_sample_entry)
    activate Task
    Task->>Task: websocket_sample_entry()
    Task->>Task: osThreadNew(websocket_sample_wifi_init)
    Note over Task: 创建WebSocket任务wifi初始化线程
    
    Task->>WiFi: wifi_register_event_cb(&wifi_event_cb)
    Note over WiFi: 注册Wi-Fi事件回调
    
    loop 等待Wi-Fi初始化
        Task->>WiFi: wifi_is_wifi_inited()
        WiFi-->>Task: 返回初始化状态
        Task->>Task: osDelay(10)
    end
    
    Task->>State: example_sta_function()
    activate State
    
    State->>WiFi: wifi_sta_enable()
    WiFi-->>State: 返回使能结果
    
    loop 状态机主循环
        State->>State: osDelay(1)
        
        alt 状态 = INIT
            State->>State: 更新状态为SCANING
            State->>WiFi: wifi_sta_scan()
            WiFi-->>State: 返回扫描结果
            
            Note over WiFi,Cb: 扫描完成后触发回调
            WiFi->>Cb: wifi_scan_state_changed()
            activate Cb
            Cb->>State: 更新状态为SCAN_DONE
            deactivate Cb
            
        else 状态 = SCAN_DONE
            State->>State: example_get_match_network()
            activate State
            State->>WiFi: wifi_sta_get_scan_info()
            WiFi-->>State: 返回扫描信息
            State->>State: 查找匹配的AP
            State-->>State: 返回匹配结果
            deactivate State
            
            alt 找到匹配AP
                State->>State: 更新状态为FOUND_TARGET
            else 未找到匹配AP
                State->>State: 更新状态为INIT
            end
            
        else 状态 = FOUND_TARGET
            State->>State: 更新状态为CONNECTING
            State->>WiFi: wifi_sta_connect(&expected_bss)
            WiFi-->>State: 返回连接结果
            
            Note over WiFi,Cb: 连接完成后触发回调
            WiFi->>Cb: wifi_connection_changed()
            activate Cb
            alt 连接成功
                Cb->>State: 更新状态为CONNECT_DONE
            else 连接失败
                Cb->>State: 更新状态为INIT
            end
            deactivate Cb
            
        else 状态 = CONNECT_DONE
            State->>State: 更新状态为GET_IP
            State->>DHCP: netifapi_netif_find("wlan0")
            DHCP-->>State: 返回网络接口
            State->>DHCP: netifapi_dhcp_start(netif_p)
            DHCP-->>State: 返回DHCP启动结果
            
        else 状态 = GET_IP
            State->>State: example_check_dhcp_status()
            activate State
            State->>DHCP: 检查IP地址是否获取
            DHCP-->>State: 返回IP状态
            alt 获取IP成功
                State-->>State: 返回成功
                State->>State: break循环
            else 获取IP超时
                State->>State: 更新状态为INIT
            else 继续等待
                State->>State: wait_count++
            end
            deactivate State
        end
    end
    
    State-->>Task: 返回成功/失败
    deactivate State
    
    Task-->>App: 返回成功/失败
    deactivate Task
```

## 时序流程说明

1. **应用启动流程**：
   - 通过`app_run(websocket_sample_entry)`启动应用
   - 创建`websocket_sample_task`线程执行`websocket_sample_init`函数

2. **WiFi初始化阶段**：
   - 注册WiFi事件回调函数
   - 等待WiFi初始化完成
   - 调用`example_sta_function()`开始STA连接流程

3. **WiFi状态机流程**：
   - **初始状态**：使能STA接口并开始扫描
   - **扫描状态**：扫描完成后触发`wifi_scan_state_changed`回调
   - **查找AP**：从扫描结果中匹配目标SSID
   - **连接阶段**：连接目标AP，完成后触发`wifi_connection_changed`回调
   - **获取IP**：启动DHCP获取IP地址
   - **完成连接**：成功获取IP后完成整个流程

4. **回调处理机制**：
   - 扫描完成回调更新状态为`SCAN_DONE`
   - 连接结果回调更新状态为`CONNECT_DONE`或失败时重置为`INIT`

5. **错误处理机制**：
   - 各阶段失败时回到初始状态重新开始
   - DHCP超时后重新从扫描开始

