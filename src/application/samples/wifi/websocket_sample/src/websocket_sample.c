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
#include "WebSocket.h"

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
#define WEBSOCKET_URL "wss://echo.websocket.org/"

// 网络连接状态标志
static volatile td_bool g_network_connected = TD_FALSE;

// 信号量定义
static osSemaphoreId_t g_network_ready_sem = NULL;

// WebSocket任务声明
static void websocket_task_entry(void *arg);
static void websocket_receive_task_entry(void *arg);
static void websocket_send_task_entry(void *arg);

// 全局WebSocket句柄，用于任务间共享
static networkHandles g_websocket_net = {0};
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
 * @param net 网络句柄
 * @param data 接收到的数据
 * @param len 数据长度
 * @details 安全地处理接收到的WebSocket消息，并打印日志
 */
static void websocket_message_callback(networkHandles *net, char *data, size_t len)
{
    UNUSED(net);
    if (data != NULL && len > 0 && len <= 1024) {
        // 安全地处理接收到的消息
        char *echo_message = osal_kmalloc(len + 1, OSAL_GFP_ATOMIC);
        if (echo_message) {
            if (memcpy_s(echo_message, len + 1, data, len) == 0) {
                echo_message[len] = '\0';
                PRINT("%s::接收回调: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, echo_message);
            } else {
                PRINT("%s::复制接收数据失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            }
            osal_kfree(echo_message);
        } else {
            PRINT("%s::分配内存失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
        }
    } else {
        PRINT("%s::接收到无效数据，长度: %zu\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, len);
    }
}

/**
 * @brief WebSocket接收任务 - 专门处理消息接收
 * @param arg 未使用
 * @details 独立任务，专门负责检查和处理WebSocket接收消息
 */
static void websocket_receive_task_entry(void *arg)
{
    UNUSED(arg);

    PRINT("%s::WebSocket接收任务启动\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    while (1) {
        // 等待WebSocket连接建立
        osSemaphoreAcquire(g_websocket_ready_sem, osWaitForever);

        PRINT("%s::开始监听WebSocket消息...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

        // 持续监听消息
        while (g_websocket_connected && g_network_connected) {
            // 非阻塞检查是否有数据可读
            char c;
            if (WebSocket_getch(&g_websocket_net, &c) > 0) {
                // 有数据到达，处理完整的帧
                size_t frame_pos = WebSocket_framePos();
                if (frame_pos > 0) {
                    size_t actual_len = 0;
                    char *received_data = WebSocket_getdata(&g_websocket_net, 512, &actual_len);
                    if (received_data != NULL && actual_len > 0) {
                        websocket_message_callback(&g_websocket_net, received_data, actual_len);
                    }
                    // 重置帧位置
                    WebSocket_framePosSeekTo(0);
                }
            }

            // 短暂延时，避免CPU占用过高
            osDelay(10); // 10ms
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

            // 准备发送数据
            char *send_buf = message;
            size_t send_len = strlen(message);
            PacketBuffers bufs = {0};

            // 发送消息
            PRINT("%s::发送: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, message);
            int rc = WebSocket_putdatas(&g_websocket_net, &send_buf, &send_len, &bufs);
            if (rc != 0) {
                PRINT("%s::发送消息失败，错误码: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, rc);
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

            // 安全地重置WebSocket句柄
            memset_s(&g_websocket_net, sizeof(networkHandles), 0, sizeof(networkHandles));

            // 初始化关键字段为安全值
            g_websocket_net.socket = -1;   // 无效socket值
            g_websocket_net.websocket = 0; // 未升级为WebSocket
            g_websocket_net.websocket_key = NULL;
            g_websocket_net.http_proxy = NULL;
            g_websocket_net.http_proxy_auth = NULL;
            g_websocket_net.httpHeaders = NULL;
#if defined(OPENSSL) || defined(MBEDTLS)
            g_websocket_net.ssl = NULL;
            g_websocket_net.ctx = NULL;
            g_websocket_net.https_proxy = NULL;
            g_websocket_net.https_proxy_auth = NULL;
#endif

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

            int rc;
            // 尝试建立WebSocket连接，添加更详细的错误处理
            PRINT("%s::调用WebSocket_connect...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

            rc = WebSocket_connect(&g_websocket_net, 0, url, false);

            PRINT("%s::WebSocket_connect返回码: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, rc);

            if (rc != 0) {
                PRINT("%s::建立WebSocket连接失败，错误码: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, rc);

                // 根据错误码提供更详细的错误信息
                switch (rc) {
                    case -1:
                        PRINT("%s::连接错误：网络不可达或连接被拒绝\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                        break;
                    case -2:
                        PRINT("%s::连接错误：DNS解析失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                        break;
                    case -3:
                        PRINT("%s::连接错误：SSL握手失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                        break;
                    case -4:
                        PRINT("%s::连接错误：WebSocket握手失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                        break;
                    default:
                        PRINT("%s::连接错误：未知错误 (%d)\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, rc);
                        break;
                }

                // 安全清理可能的残留状态
                if (g_websocket_net.socket > 0) {
                    // 如果socket已分配，尝试关闭它
                    PRINT("%s::清理残留socket: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, g_websocket_net.socket);
                }

                // 重新初始化句柄
                memset_s(&g_websocket_net, sizeof(networkHandles), 0, sizeof(networkHandles));
                g_websocket_net.socket = -1;

                osDelay(5000); // 延迟5秒后重试
                continue;
            }

            PRINT("%s::WebSocket连接已建立到 %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, url);

            // 验证连接句柄有效性
            if (g_websocket_net.socket <= 0) {
                PRINT("%s::WebSocket socket无效: %d，重试连接\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, g_websocket_net.socket);
                memset_s(&g_websocket_net, sizeof(networkHandles), 0, sizeof(networkHandles));
                g_websocket_net.socket = -1;
                continue;
            }

            // 验证WebSocket升级状态
            if (g_websocket_net.websocket != 1) {
                PRINT("%s::WebSocket未正确升级，状态: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, g_websocket_net.websocket);
                WebSocket_close(&g_websocket_net, WebSocket_CLOSE_NORMAL, "升级失败");
                memset_s(&g_websocket_net, sizeof(networkHandles), 0, sizeof(networkHandles));
                g_websocket_net.socket = -1;
                continue;
            }

            PRINT("%s::WebSocket连接验证通过，socket: %d, websocket: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG,
                  g_websocket_net.socket, g_websocket_net.websocket);

            // 设置WebSocket连接状态
            g_websocket_connected = TD_TRUE;

            // 通知收发任务开始工作（释放两次信号量）
            if (g_websocket_ready_sem != NULL) {
                osSemaphoreRelease(g_websocket_ready_sem);
                osSemaphoreRelease(g_websocket_ready_sem);
                PRINT("%s::已通知收发任务开始工作\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            } else {
                PRINT("%s::警告：WebSocket信号量为空\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            }

            // 监控连接状态
            while (g_network_connected && g_websocket_connected) {
                osDelay(1000); // 每秒检查一次连接状态

                // 检查socket是否仍然有效
                if (g_websocket_net.socket <= 0) {
                    PRINT("%s::检测到WebSocket socket异常: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, g_websocket_net.socket);
                    g_websocket_connected = TD_FALSE;
                    break;
                }

                // 可选：添加简单的心跳检测
                // 这里可以发送ping帧来检测连接活性
            }

            // 连接断开处理
            PRINT("%s::WebSocket连接断开，开始清理...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            g_websocket_connected = TD_FALSE;

            // 安全关闭WebSocket连接
            if (g_websocket_net.socket > 0 && g_websocket_net.websocket == 1) {
                PRINT("%s::正在关闭WebSocket连接，socket: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, g_websocket_net.socket);
                WebSocket_close(&g_websocket_net, WebSocket_CLOSE_NORMAL, "正常关闭");
            } else {
                PRINT("%s::跳过WebSocket关闭，socket: %d, websocket: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG,
                      g_websocket_net.socket, g_websocket_net.websocket);
            }

            // 终止WebSocket（清理全局状态）
            WebSocket_terminate();

            // 完全清理句柄
            memset_s(&g_websocket_net, sizeof(networkHandles), 0, sizeof(networkHandles));
            g_websocket_net.socket = -1;

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