/**
 * @copyright (c) sanchuanhehe
 *
 * @file sta_sample.c
 * @brief 实现STA模式下的Wi-Fi WebSocket示例
 * @history
 * 2022-07-27, Create file.
 */

#include "lwip/netifapi.h"
#include "std_def.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "librws.h"

#define WIFI_IFNAME_MAX_SIZE 16
#define WIFI_MAX_SSID_LEN 33
#define WIFI_SCAN_AP_LIMIT 64
#define WIFI_MAC_LEN 6
#define WIFI_WEBSOCKET_SAMPLE_LOG "[WIFI_WEBSOCKET_SAMPLE]"
#define WIFI_NOT_AVALLIABLE 0
#define WIFI_AVALIABE 1
#define WIFI_GET_IP_MAX_COUNT 300
#define WIFI_SSID "my_softAP"  /**< 待连接的网络接入名称 */
#define WIFI_PSK "my_password" /**< 待连接的网络接入密码 */

#define WIFI_TASK_PRIO (osPriority_t)(13)
#define WIFI_TASK_DURATION_MS 2000
#define WIFI_TASK_STACK_SIZE 0x1000

// WebSocket任务相关定义
#define WEBSOCKET_TASK_STACK_SIZE 0x1000
#define WEBSOCKET_TASK_PRIO (osPriority_t)(12) // 低于WiFi任务优先级
#define WEBSOCKET_URL "ws://192.168.18.149:8765"

// 网络连接状态标志
static volatile td_bool g_network_connected = TD_FALSE;

// 信号量定义
static osSemaphoreId_t g_network_ready_sem = NULL;

// WebSocket任务声明
static void websocket_task_entry(void *arg);
static void websocket_receive_task_entry(void *arg);
static void websocket_send_task_entry(void *arg);

// 全局WebSocket句柄，用于任务间共享
static rws_socket g_websocket = NULL;
static volatile td_bool g_websocket_connected = TD_FALSE;
static osSemaphoreId_t g_websocket_ready_sem = NULL;

/**
 * @brief Wi-Fi状态机枚举定义
 */
enum {
    WIFI_WEBSOCKET_SAMPLE_INIT = 0,     /**< 初始态 */
    WIFI_WEBSOCKET_SAMPLE_SCANING,      /**< 扫描中 */
    WIFI_WEBSOCKET_SAMPLE_SCAN_DONE,    /**< 扫描完成 */
    WIFI_WEBSOCKET_SAMPLE_FOUND_TARGET, /**< 匹配到目标AP */
    WIFI_WEBSOCKET_SAMPLE_CONNECTING,   /**< 连接中 */
    WIFI_WEBSOCKET_SAMPLE_CONNECT_DONE, /**< 关联成功 */
    WIFI_WEBSOCKET_SAMPLE_GET_IP,       /**< 获取IP */
} wifi_state_enum;

static td_u8 g_wifi_state = WIFI_WEBSOCKET_SAMPLE_INIT;

// 事件回调声明
static td_void wifi_scan_state_changed(td_s32 state, td_s32 size);
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code);

// 事件回调结构体
wifi_event_stru wifi_event_cb = {.wifi_event_connection_changed = wifi_connection_changed,
                                 .wifi_event_scan_state_changed = wifi_scan_state_changed};

/**
 * @brief STA 扫描事件回调函数
 * @param state 扫描状态
 * @param size  扫描结果数量
 * @details 扫描完成后设置状态
 */
static td_void wifi_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    UNUSED(size);
    PRINT("%s::Scan done!.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    g_wifi_state = WIFI_WEBSOCKET_SAMPLE_SCAN_DONE;
    return;
}

/**
 * @brief STA 关联事件回调函数
 * @param state       连接状态
 * @param info        连接信息
 * @param reason_code 失败原因码
 * @details 连接成功或失败时设置状态
 */
static td_void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(info);
    UNUSED(reason_code);

    if (state == WIFI_NOT_AVALLIABLE) {
        PRINT("%s::Connect fail!. try agin !\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        g_wifi_state = WIFI_WEBSOCKET_SAMPLE_INIT;
        // 网络断开时更新标志
        g_network_connected = TD_FALSE;
    } else {
        PRINT("%s::Connect succ!.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        g_wifi_state = WIFI_WEBSOCKET_SAMPLE_CONNECT_DONE;
    }
}

/**
 * @brief STA 匹配目标AP
 * @param expected_bss [out] 匹配到的AP信息
 * @return 0: 成功, -1: 失败
 * @details 从扫描结果中查找指定SSID的AP，并填充连接参数
 */
td_s32 example_get_match_network(wifi_sta_config_stru *expected_bss)
{
    td_s32 ret;
    td_u32 num = WIFI_SCAN_AP_LIMIT;
    td_char *expected_ssid = WIFI_SSID;
    td_char *key = WIFI_PSK;
    td_bool find_ap = TD_FALSE;
    td_u8 bss_index;
    td_u32 scan_len = sizeof(wifi_scan_info_stru) * WIFI_SCAN_AP_LIMIT;
    wifi_scan_info_stru *result = osal_kmalloc(scan_len, OSAL_GFP_ATOMIC);
    if (result == TD_NULL) {
        return -1;
    }
    memset_s(result, scan_len, 0, scan_len);
    ret = wifi_sta_get_scan_info(result, &num);
    if (ret != 0) {
        osal_kfree(result);
        return -1;
    }
    // 遍历扫描结果，查找目标SSID
    for (bss_index = 0; bss_index < num; bss_index++) {
        if (strlen(expected_ssid) == strlen(result[bss_index].ssid)) {
            if (memcmp(expected_ssid, result[bss_index].ssid, strlen(expected_ssid)) == 0) {
                find_ap = TD_TRUE;
                break;
            }
        }
    }
    // 未找到目标AP
    if (find_ap == TD_FALSE) {
        osal_kfree(result);
        return -1;
    }
    // 填充连接参数
    if (memcpy_s(expected_bss->ssid, WIFI_MAX_SSID_LEN, expected_ssid, strlen(expected_ssid)) != 0) {
        osal_kfree(result);
        return -1;
    }
    if (memcpy_s(expected_bss->bssid, WIFI_MAC_LEN, result[bss_index].bssid, WIFI_MAC_LEN) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->security_type = result[bss_index].security_type;
    if (memcpy_s(expected_bss->pre_shared_key, WIFI_MAX_SSID_LEN, key, strlen(key)) != 0) {
        osal_kfree(result);
        return -1;
    }
    expected_bss->ip_type = 1; // 1：IP类型为动态DHCP获取
    osal_kfree(result);
    return 0;
}

/**
 * @brief STA 关联状态查询
 * @retval 0 连接成功
 * @retval -1 连接失败
 * @details 查询连接状态，最多尝试5次
 */
td_bool example_check_connect_status(td_void)
{
    td_u8 index;
    wifi_linked_info_stru wifi_status;
    for (index = 0; index < 5; index++) {
        (void)osDelay(50); // 50: 延时500ms
        memset_s(&wifi_status, sizeof(wifi_linked_info_stru), 0, sizeof(wifi_linked_info_stru));
        if (wifi_sta_get_ap_info(&wifi_status) != 0) {
            continue;
        }
        if (wifi_status.conn_state == 1) {
            return 0; // 连接成功
        }
    }
    return -1;
}

/**
 * @brief STA DHCP状态查询
 * @param netif_p   [in] 网络接口指针
 * @param wait_count [in/out] 等待计数
 * @retval 0 获取IP成功
 * @retval -1 获取IP失败
 * @details 检查DHCP是否成功获取IP
 */
td_bool example_check_dhcp_status(struct netif *netif_p, td_u32 *wait_count)
{
    if ((ip_addr_isany(&(netif_p->ip_addr)) == 0) && (*wait_count <= WIFI_GET_IP_MAX_COUNT)) {
        PRINT("%s::STA DHCP success.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

        // 设置网络连接标志并释放信号量
        g_network_connected = TD_TRUE;
        if (g_network_ready_sem != NULL) {
            osSemaphoreRelease(g_network_ready_sem);
        }

        return 0;
    }
    if (*wait_count > WIFI_GET_IP_MAX_COUNT) {
        PRINT("%s::STA DHCP timeout, try again !.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        *wait_count = 0;
        g_wifi_state = WIFI_WEBSOCKET_SAMPLE_INIT;
    }
    return -1;
}

/**
 * @brief STA 主流程函数
 * @retval 0 成功
 * @retval -1 失败
 * @details 包含状态机，依次完成STA使能、扫描、连接、DHCP获取IP等流程
 */
td_s32 example_sta_function(td_void)
{
    td_char ifname[WIFI_IFNAME_MAX_SIZE + 1] = "wlan0"; // STA接口名
    wifi_sta_config_stru expected_bss = {0};            // 连接请求信息
    struct netif *netif_p = TD_NULL;
    td_u32 wait_count = 0;

    // 1. 使能STA接口
    if (wifi_sta_enable() != 0) {
        return -1;
    }
    PRINT("%s::STA enable succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    // 2. 状态机主循环
    do {
        (void)osDelay(1); // 等待10ms后判断状态
        if (g_wifi_state == WIFI_WEBSOCKET_SAMPLE_INIT) {
            PRINT("%s::Scan start!\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            g_wifi_state = WIFI_WEBSOCKET_SAMPLE_SCANING;
            // 启动STA扫描
            if (wifi_sta_scan() != 0) {
                g_wifi_state = WIFI_WEBSOCKET_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_WEBSOCKET_SAMPLE_SCAN_DONE) {
            // 获取待连接的网络
            if (example_get_match_network(&expected_bss) != 0) {
                PRINT("%s::Do not find AP, try again !\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                g_wifi_state = WIFI_WEBSOCKET_SAMPLE_INIT;
                continue;
            }
            g_wifi_state = WIFI_WEBSOCKET_SAMPLE_FOUND_TARGET;
        } else if (g_wifi_state == WIFI_WEBSOCKET_SAMPLE_FOUND_TARGET) {
            PRINT("%s::Connect start.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            g_wifi_state = WIFI_WEBSOCKET_SAMPLE_CONNECTING;
            // 启动连接
            if (wifi_sta_connect(&expected_bss) != 0) {
                g_wifi_state = WIFI_WEBSOCKET_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_WEBSOCKET_SAMPLE_CONNECT_DONE) {
            PRINT("%s::DHCP start.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            g_wifi_state = WIFI_WEBSOCKET_SAMPLE_GET_IP;
            netif_p = netifapi_netif_find(ifname);
            if (netif_p == TD_NULL || netifapi_dhcp_start(netif_p) != 0) {
                PRINT("%s::find netif or start DHCP fail, try again !\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                g_wifi_state = WIFI_WEBSOCKET_SAMPLE_INIT;
                continue;
            }
        } else if (g_wifi_state == WIFI_WEBSOCKET_SAMPLE_GET_IP) {
            // 检查DHCP状态
            if (example_check_dhcp_status(netif_p, &wait_count) == 0) {
                break;
            }
            wait_count++;
        }
    } while (1);

    return 0;
}

/**
 * @brief websocket_sample_wifi_init
 * @param param 未使用
 * @retval 0 成功
 * @retval -1 失败
 * @details 入口函数，注册事件回调，等待WiFi初始化，启动主流程
 */
int websocket_sample_wifi_init(void *param)
{
    UNUSED(param);
    // 创建网络就绪信号量
    g_network_ready_sem = osSemaphoreNew(1, 0, NULL);
    if (g_network_ready_sem == NULL) {
        PRINT("%s::Create network semaphore fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        return -1;
    }
    // 注册事件回调
    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        PRINT("%s::wifi_event_cb register fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        return -1;
    }
    PRINT("%s::wifi_event_cb register succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    // 等待wifi初始化完成
    while (wifi_is_wifi_inited() == 0) {
        (void)osDelay(10); // 等待100ms后判断状态
    }
    PRINT("%s::wifi init succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    // 启动STA主流程
    if (example_sta_function() != 0) {
        PRINT("%s::example_sta_function fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        return -1;
    }
    return 0;
}

/**
 * @brief WebSocket消息回调函数
 * @param socket WebSocket句柄
 * @param text 接收到的文本数据
 * @param length 数据长度
 * @details 安全地处理接收到的WebSocket消息，并打印日志
 */
static void websocket_message_callback(rws_socket socket, const char *text, const unsigned int length)
{
    UNUSED(socket);
    if (text != NULL && length > 0 && length <= 1024) {
        // 安全地处理接收到的消息
        char *echo_message = osal_kmalloc(length + 1, OSAL_GFP_ATOMIC);
        if (echo_message) {
            if (memcpy_s(echo_message, length + 1, text, length) == 0) {
                echo_message[length] = '\0';
                PRINT("%s::接收回调: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, echo_message);
            } else {
                PRINT("%s::复制接收数据失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            }
            osal_kfree(echo_message);
        } else {
            PRINT("%s::分配内存失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        }
    } else {
        PRINT("%s::接收到无效数据，长度: %u\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, length);
    }
}

/**
 * @brief WebSocket连接成功回调函数
 * @param socket WebSocket句柄
 */
static void websocket_on_connected(rws_socket socket)
{
    UNUSED(socket);
    PRINT("%s::WebSocket连接成功\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    g_websocket_connected = TD_TRUE;
    
    // 通知收发任务开始工作（释放两次信号量）
    if (g_websocket_ready_sem != NULL) {
        osSemaphoreRelease(g_websocket_ready_sem);
        osSemaphoreRelease(g_websocket_ready_sem);
        PRINT("%s::已通知收发任务开始工作\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
}

/**
 * @brief WebSocket断开连接回调函数
 * @param socket WebSocket句柄
 */
static void websocket_on_disconnected(rws_socket socket)
{
    UNUSED(socket);
    PRINT("%s::WebSocket连接断开\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    g_websocket_connected = TD_FALSE;
}

/**
 * @brief WebSocket接收任务 - 专门处理消息接收
 * @param arg 未使用
 * @details 使用librws的回调机制，此任务仅用于等待连接和维护状态
 */
static void websocket_receive_task_entry(void *arg)
{
    UNUSED(arg);

    PRINT("%s::WebSocket接收任务启动\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    while (1) {
        // 等待WebSocket连接建立
        osSemaphoreAcquire(g_websocket_ready_sem, osWaitForever);

        PRINT("%s::开始监听WebSocket消息...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

        // 持续监听连接状态 - librws通过回调处理消息接收
        while (g_websocket_connected && g_network_connected) {
            // librws自动处理消息接收，我们只需要检查连接状态
            if (g_websocket != NULL && !rws_socket_is_connected(g_websocket)) {
                PRINT("%s::检测到WebSocket连接断开\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                g_websocket_connected = TD_FALSE;
                break;
            }

            // 短暂延时，避免CPU占用过高
            osDelay(100); // 100ms
        }

        PRINT("%s::WebSocket接收任务暂停\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
}

/**
 * @brief WebSocket发送任务 - 专门处理消息发送
 * @param arg 未使用
 * @details 独立任务，专门负责定期发送WebSocket消息
 */
static void websocket_send_task_entry(void *arg)
{
    UNUSED(arg);

    PRINT("%s::WebSocket发送任务启动\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    int message_count = 1;

    while (1) {
        // 等待WebSocket连接建立
        osSemaphoreAcquire(g_websocket_ready_sem, osWaitForever);

        PRINT("%s::开始发送WebSocket消息...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

        // 持续发送消息
        while (g_websocket_connected && g_network_connected) {
            // 创建要发送的消息
            char message[128];
            snprintf(message, sizeof(message), "来自IoT设备的消息 #%d", message_count++);

            // 使用librws发送文本消息
            PRINT("%s::发送: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, message);
            rws_bool send_result = rws_socket_send_text(g_websocket, message);
            if (!send_result) {
                PRINT("%s::发送消息失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                g_websocket_connected = TD_FALSE;
                break;
            }

            // 等待5秒后发送下一条消息
            osDelay(5000);
        }

        PRINT("%s::WebSocket发送任务暂停\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
}

/**
 * @brief WebSocket连接管理任务
 * @param arg 未使用
 * @details 负责建立和管理WebSocket连接，协调收发任务
 */
static void websocket_task_entry(void *arg)
{
    UNUSED(arg);

    PRINT("%s::WebSocket管理任务启动，等待网络连接...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    while (1) {
        // 等待网络连接就绪信号
        osSemaphoreAcquire(g_network_ready_sem, osWaitForever);

        // 确认网络已连接
        if (g_network_connected) {
            PRINT("%s::网络已连接，开始WebSocket连接...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

            // 清理之前的WebSocket连接
            if (g_websocket != NULL) {
                rws_socket_disconnect_and_release(g_websocket);
                g_websocket = NULL;
            }

            // 验证URL有效性
            const char *url = WEBSOCKET_URL;
            if (url == NULL || strlen(url) == 0) {
                PRINT("%s::WebSocket URL无效\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                osDelay(5000);
                continue;
            }

            PRINT("%s::准备连接到: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, url);

            // 添加延时，确保网络完全稳定
            osDelay(1000);

            // 再次检查网络状态
            if (!g_network_connected) {
                PRINT("%s::网络已断开，取消WebSocket连接\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                continue;
            }

            // 创建新的WebSocket连接
            g_websocket = rws_socket_create();
            if (g_websocket == NULL) {
                PRINT("%s::创建WebSocket失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                osDelay(5000);
                continue;
            }            // 解析URL并设置连接参数
            // 简单解析 wss://echo.websocket.org/
            const char *scheme = "ws";
            char host_buffer[256] = {0};
            const char *host = host_buffer;
            int port = 80;
            const char *path = "/";
            
            // 检查是否是wss协议
            const char *host_start;
            if (strncmp(url, "wss://", 6) == 0) {
                scheme = "ws"; // librws中使用"ws"表示WebSocket，SSL通过端口判断
                host_start = url + 6; // 跳过"wss://"
                port = 443;
            } else if (strncmp(url, "ws://", 5) == 0) {
                scheme = "ws";
                host_start = url + 5; // 跳过"ws://"
                port = 80;
            } else {
                PRINT("%s::不支持的URL协议: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, url);
                rws_socket_disconnect_and_release(g_websocket);
                g_websocket = NULL;
                osDelay(5000);
                continue;
            }

            // 查找路径部分
            const char *path_start = strchr(host_start, '/');
            if (path_start != NULL) {
                // 复制主机名部分
                size_t host_len = path_start - host_start;
                if (host_len >= sizeof(host_buffer)) {
                    host_len = sizeof(host_buffer) - 1;
                }
                strncpy(host_buffer, host_start, host_len);
                host_buffer[host_len] = '\0';
                path = path_start;
            } else {
                // 没有路径，整个是主机名
                strncpy(host_buffer, host_start, sizeof(host_buffer) - 1);
                host_buffer[sizeof(host_buffer) - 1] = '\0';
                path = "/";
            }

            // 设置连接参数
            rws_socket_set_url(g_websocket, scheme, host, port, path);

            // 设置回调函数
            rws_socket_set_on_connected(g_websocket, websocket_on_connected);
            rws_socket_set_on_disconnected(g_websocket, websocket_on_disconnected);
            rws_socket_set_on_received_text(g_websocket, websocket_message_callback);

            PRINT("%s::调用rws_socket_connect...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

            // 尝试建立WebSocket连接
            rws_bool connect_result = rws_socket_connect(g_websocket);

            if (!connect_result) {
                PRINT("%s::建立WebSocket连接失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                
                // 检查错误信息
                rws_error error = rws_socket_get_error(g_websocket);
                if (error != NULL) {
                    int error_code = rws_error_get_code(error);
                    const char *error_desc = rws_error_get_description(error);
                    PRINT("%s::错误码: %d, 描述: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, 
                          error_code, error_desc ? error_desc : "无描述");
                }

                // 清理失败的连接
                rws_socket_disconnect_and_release(g_websocket);
                g_websocket = NULL;

                osDelay(5000); // 延迟5秒后重试
                continue;
            }

            PRINT("%s::WebSocket连接请求已发送\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

            // 等待连接建立或失败
            int wait_count = 0;
            const int max_wait = 100; // 10秒超时 (100 * 100ms)
            
            while (wait_count < max_wait && g_network_connected) {
                if (g_websocket_connected) {
                    PRINT("%s::WebSocket连接已建立\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                    break;
                }
                
                // 检查是否仍然连接中
                if (!rws_socket_is_connected(g_websocket)) {
                    // 检查是否有错误
                    rws_error error = rws_socket_get_error(g_websocket);
                    if (error != NULL) {
                        int error_code = rws_error_get_code(error);
                        const char *error_desc = rws_error_get_description(error);
                        PRINT("%s::连接失败，错误码: %d, 描述: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, 
                              error_code, error_desc ? error_desc : "无描述");
                    } else {
                        PRINT("%s::连接失败，未知错误\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                    }
                    break;
                }
                
                osDelay(100); // 等待100ms
                wait_count++;
            }

            if (!g_websocket_connected) {
                PRINT("%s::WebSocket连接超时或失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                
                // 清理失败的连接
                if (g_websocket != NULL) {
                    rws_socket_disconnect_and_release(g_websocket);
                    g_websocket = NULL;
                }
                
                osDelay(5000);
                continue;
            }

            // 监控连接状态
            while (g_network_connected && g_websocket_connected) {
                osDelay(1000); // 每秒检查一次连接状态

                // 检查连接是否仍然有效
                if (g_websocket != NULL && !rws_socket_is_connected(g_websocket)) {
                    PRINT("%s::检测到WebSocket连接断开\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                    g_websocket_connected = TD_FALSE;
                    break;
                }
            }

            // 连接断开处理
            PRINT("%s::WebSocket连接断开，开始清理...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            g_websocket_connected = TD_FALSE;

            // 安全关闭WebSocket连接
            if (g_websocket != NULL) {
                PRINT("%s::正在关闭WebSocket连接\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                rws_socket_disconnect_and_release(g_websocket);
                g_websocket = NULL;
            }

            PRINT("%s::WebSocket连接已完全清理\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        } else {
            PRINT("%s::网络未连接，等待网络恢复...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        }
    }
}

/**
 * @brief websocket_sample_wifi_entry
 * @details 创建所有WebSocket相关线程
 */
static void websocket_sample_wifi_entry(void)
{
    // 创建WebSocket连接就绪信号量（初始值为2，支持两个任务等待）
    g_websocket_ready_sem = osSemaphoreNew(2, 0, NULL);
    if (g_websocket_ready_sem == NULL) {
        PRINT("%s::Create websocket semaphore fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        return;
    }

    // 创建WiFi初始化任务
    osThreadAttr_t wifi_init_attr;
    wifi_init_attr.name = "websocket_sample_wifi_init";
    wifi_init_attr.attr_bits = 0U;
    wifi_init_attr.cb_mem = NULL;
    wifi_init_attr.cb_size = 0U;
    wifi_init_attr.stack_mem = NULL;
    wifi_init_attr.stack_size = WIFI_TASK_STACK_SIZE;
    wifi_init_attr.priority = WIFI_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)websocket_sample_wifi_init, NULL, &wifi_init_attr) == NULL) {
        PRINT("%s::Create websocket_sample_wifi_init fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
    PRINT("%s::Create websocket_sample_wifi_init succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    // 创建WebSocket管理任务
    osThreadAttr_t ws_mgr_attr;
    ws_mgr_attr.name = "websocket_manager";
    ws_mgr_attr.attr_bits = 0U;
    ws_mgr_attr.cb_mem = NULL;
    ws_mgr_attr.cb_size = 0U;
    ws_mgr_attr.stack_mem = NULL;
    ws_mgr_attr.stack_size = WEBSOCKET_TASK_STACK_SIZE;
    ws_mgr_attr.priority = WEBSOCKET_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)websocket_task_entry, NULL, &ws_mgr_attr) == NULL) {
        PRINT("%s::Create websocket_manager fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
    PRINT("%s::Create websocket_manager succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    // 创建WebSocket接收任务
    osThreadAttr_t ws_recv_attr;
    ws_recv_attr.name = "websocket_receiver";
    ws_recv_attr.attr_bits = 0U;
    ws_recv_attr.cb_mem = NULL;
    ws_recv_attr.cb_size = 0U;
    ws_recv_attr.stack_mem = NULL;
    ws_recv_attr.stack_size = WEBSOCKET_TASK_STACK_SIZE;
    ws_recv_attr.priority = WEBSOCKET_TASK_PRIO - 1; // 稍高优先级
    if (osThreadNew((osThreadFunc_t)websocket_receive_task_entry, NULL, &ws_recv_attr) == NULL) {
        PRINT("%s::Create websocket_receiver fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
    PRINT("%s::Create websocket_receiver succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    // 创建WebSocket发送任务
    osThreadAttr_t ws_send_attr;
    ws_send_attr.name = "websocket_sender";
    ws_send_attr.attr_bits = 0U;
    ws_send_attr.cb_mem = NULL;
    ws_send_attr.cb_size = 0U;
    ws_send_attr.stack_mem = NULL;
    ws_send_attr.stack_size = WEBSOCKET_TASK_STACK_SIZE;
    ws_send_attr.priority = WEBSOCKET_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)websocket_send_task_entry, NULL, &ws_send_attr) == NULL) {
        PRINT("%s::Create websocket_sender fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
    PRINT("%s::Create websocket_sender succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
}

/**
 * @brief 启动websocket_sample_wifi_init
 */
app_run(websocket_sample_wifi_entry);