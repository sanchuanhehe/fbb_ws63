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
 * @brief websocket_sample_wifi_entry
 * @details 创建websocket_sample_wifi_init线程
 */
static void websocket_sample_wifi_entry(void)
{
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
    osThreadAttr_t ws_task_attr;
    // 创建WebSocket任务
    ws_task_attr.name = "websocket_task";
    ws_task_attr.attr_bits = 0U;
    ws_task_attr.cb_mem = NULL;
    ws_task_attr.cb_size = 0U;
    ws_task_attr.stack_mem = NULL;
    ws_task_attr.stack_size = WEBSOCKET_TASK_STACK_SIZE;
    ws_task_attr.priority = WEBSOCKET_TASK_PRIO;
    if (osThreadNew((osThreadFunc_t)websocket_task_entry, NULL, &ws_task_attr) == NULL) {
        PRINT("%s::Create websocket_task fail.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
    }
    PRINT("%s::Create websocket_task succ.\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
}

/**
 * @brief WebSocket 任务实现
 * @param arg 未使用
 * @details 等待网络连接就绪，执行WebSocket操作，监控网络状态
 */
static void websocket_task_entry(void *arg)
{
    UNUSED(arg);

    PRINT("%s::WebSocket任务启动，等待网络连接...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

    while (1) {
        // 等待网络连接就绪信号
        osSemaphoreAcquire(g_network_ready_sem, osWaitForever);

        // 确认网络已连接
        if (g_network_connected) {
            PRINT("%s::网络已连接，开始WebSocket连接...\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);

            // 声明网络句柄和WebSocket URL
            networkHandles net = {0};
            const char *url = "wss://toolin.cn/echo"; // 修改为指定的URL
            int rc;

            // 尝试建立WebSocket连接
            rc = WebSocket_connect(&net, 1, url); // 1表示使用SSL
            if (rc != 0) {
                PRINT("%s::建立WebSocket连接失败，错误码: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, rc);
                osDelay(5000); // 延迟5秒后重试
                continue;
            }

            PRINT("%s::WebSocket连接已建立到 %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, url);

            // WebSocket通信循环
            int message_count = 1;
            while (g_network_connected) {
                // 创建要发送的消息
                char message[128];
                snprintf(message, sizeof(message), "来自IoT设备的消息 #%d", message_count++);

                // 准备发送数据
                char *send_buf = message;
                size_t send_len = strlen(message);
                PacketBuffers bufs = {0};

                // 发送消息
                PRINT("%s::发送: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, message);
                rc = WebSocket_putdatas(&net, &send_buf, &send_len, &bufs);
                if (rc != 0) {
                    PRINT("%s::发送消息失败，错误码: %d\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, rc);
                    break;
                }

                // 等待服务器响应
                osDelay(500);

                // 接收响应 - 增加安全检查
                size_t actual_len = 0;
                char *received_data = WebSocket_getdata(&net, 256, &actual_len);

                if (received_data != NULL && actual_len > 0 && actual_len <= 256) {
                    // 增加边界检查
                    // 将接收到的数据转换为可打印字符串
                    char *echo_message = osal_kmalloc(actual_len + 1, OSAL_GFP_ATOMIC);
                    if (echo_message) {
                        // 使用安全的内存操作
                        if (memcpy_s(echo_message, actual_len + 1, received_data, actual_len) == 0) {
                            echo_message[actual_len] = '\0';
                            PRINT("%s::接收: %s\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, echo_message);
                        } else {
                            PRINT("%s::复制接收数据失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                        }
                        osal_kfree(echo_message);
                    } else {
                        PRINT("%s::分配内存失败\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                    }
                } else {
                    // 详细记录错误情况
                    if (received_data == NULL) {
                        PRINT("%s::接收数据为空\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                    } else if (actual_len == 0) {
                        PRINT("%s::接收数据长度为0\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
                    } else {
                        PRINT("%s::接收数据长度异常: %zu\r\n", WIFI_WEBSOCKET_SAMPLE_LOG, actual_len);
                    }
                }

                // 等待5秒后发送下一条消息
                osDelay(5000);
            }

            // 关闭WebSocket连接
            PRINT("%s::关闭WebSocket连接\r\n", WIFI_WEBSOCKET_SAMPLE_LOG);
            WebSocket_close(&net, WebSocket_CLOSE_NORMAL, "正常关闭");
            WebSocket_terminate();
        }
    }
}

/**
 * @brief 启动websocket_sample_wifi_init
 */
app_run(websocket_sample_wifi_entry);