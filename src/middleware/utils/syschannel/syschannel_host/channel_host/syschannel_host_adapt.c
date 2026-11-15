/* 头文件包含 */
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
#include "wal_netlink.h"
#include "oal_netdev.h"
#endif
#include "osal_adapt.h"
#include "wal_net.h"
#include "hcc_cfg.h"
#include "syschannel_host_adapt.h"
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
#include "hal_reboot.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SYSCHANNEL_HCC_UNC_POOL_SIZE 80
#define SYSCHANNEL_HCC_UNC_POOL_SIZE_LOW_LIMIT 4
#define HOST_HCC_TASK_PRIORITY 10
#define HCC_TX_ASEEMBLE_CNT 4
#define BSP_READY_WAIT_MS 50
#define BSP_READY_RETRY_TIMES 10

osal_u8    g_queue_flow_flag = OSAL_FALSE;
static osal_timer g_heartbeat_host_timer;
static osal_u8 g_heartbeat_host_cnt = 0;
syschannel_handler g_syschannel_handler_host[1] = {0};
static osal_bool g_bsp_ready = OSAL_FALSE;
static osal_wait g_bsp_ready_wait;
static osal_bool g_suspend_ack = OSAL_FALSE;
static osal_wait g_suspend_ack_wait;
/* 如果需要支持HOST侧自动恢复探卡功能，则将SYSCHANNEL_SUPPORT_AUTO_RESUME配置为1，默认不打开，由业务实现恢复逻辑 */
#define SYSCHANNEL_SUPPORT_AUTO_RESUME 0
#if defined(SYSCHANNEL_SUPPORT_AUTO_RESUME) && (SYSCHANNEL_SUPPORT_AUTO_RESUME != 0)
static osal_task *g_resume_task;
static osal_bool g_resume_wake = OSAL_FALSE;
static osal_wait g_resume_wait;
#define SYSCHANNEL_RESUME_TASK_STACK 0x800
#define SYSCHANNEL_RESUME_TASK_PRIO 26
#define SYSCHANNEL_RESUME_MAX_RETRY 500
#define SYSCHANNEL_RESUME_MAX_DELAY 10
#endif
static osal_u32 syschannel_wait_suspend_ack(osal_u8 *cb_data);

enum {
    SYSCHANNEL_NOT_INIT = 0,            /* not initial */
    SYSCHANNEL_INIT = 1,                /* initial */
    SYSCHANNEL_SUSPEND = 2,             /* suspend */
    SYSCHANNEL_RESUME = 3,              /* resume */
} syschannel_status;

static osal_u32 g_syschannel_status = SYSCHANNEL_NOT_INIT;

syschannel_handler *syschannel_host_get_handler(osal_void)
{
    return &g_syschannel_handler_host[0];
}

static inline osal_void syschannel_host_clear_heartbeat_count(osal_void)
{
    g_heartbeat_host_cnt = 0;
}

/* 回心跳报文ack帧 */
osal_u32 syschannel_heartbeat_send_ack(osal_void)
{
    osal_u32 ret;
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        osal_printk("syschannel_handler is inuse\r\n");
        return OSAL_NOK;
    }

    ret = hcc_send_message(syschannel_handler->hcc_id, H2D_MSG_HEARTBEAT_ACK, 0);
    if (ret != OSAL_OK) {
        osal_printk("syschannel_heartbeat_send_ack tx failed\r\n");
        return ret;
    }

    return OSAL_OK;
}

static osal_u32 syschannel_d2h_headbeat(osal_u8 *cb_data)
{
    unref_param(cb_data);
    /* 清零心跳计数 */
    syschannel_host_clear_heartbeat_count();
    /* 回ack给device */
    if (syschannel_heartbeat_send_ack() != OSAL_OK) {
        return OSAL_NOK;
    }
    return 0;
}

static osal_s32  syschannel_bsp_ready_condition(const void *param)
{
    unref_param(param);
    return g_bsp_ready;
}

static osal_u32 syschannel_wait_bsp_ready(osal_u8 *cb_data)
{
    g_bsp_ready = OSAL_TRUE;
    osal_wait_wakeup((osal_wait *)cb_data);
    return 0;
}

static osal_u32 syschannel_host_enable_heartbeat_ack(osal_u8 *buf, osal_u32 len)
{
    if (len <= SYSCHANNEL_MSG_TYPE_LEN) {
        return OSAL_NOK;
    }
    /* 清零心跳计数 */
    syschannel_host_clear_heartbeat_count();
    /* 使能则启动心跳收包定时器, 去使能则停定时器 */
    if (buf[1] == HB_ENABLE) {
        syschannel_heartbeat_timer_create();
    } else if (buf[1] == HB_DISABLE) {
        syschannel_heartbeat_timer_destroy();
    }
    return OSAL_OK;
}

static osal_u32 syschannel_host_rx_msg_mnt(osal_u8 *buf, osal_u32 len)
{
    osal_u8 msg_mnt_type = 0xff;
    if ((buf == OSAL_NULL) || (len == 0)) {
        osal_printk("syschannel_host_rx_msg_mnt is fail:len:%d\n", len);
        return OSAL_NOK;
    }
    msg_mnt_type = buf[0];
    switch (msg_mnt_type) {
        case SYSCHANNEL_HEARTBEAT_ENABLE_ACK:
            syschannel_host_enable_heartbeat_ack(buf, len);
            break;
        case SYSCHANNEL_HOST_SYNC_MAC_ADDR:
            syschannel_host_sync_mac_addr(buf, len);
            break;
        case SYSCHANNEL_HOST_SYNC_IP_ADDR:
            syschannel_host_sync_ip_addr(buf, len);
            break;
        default:
            break;
    }
    return OSAL_OK;
}

STATIC osal_u32 syschannel_host_rx_msg_process(hcc_queue_type queue_id, osal_u8 stype,
    osal_u8 *buf, osal_u32 len, osal_u8 *user_param)
{
    unref_param(queue_id);
    unref_param(user_param);
    osal_u16 data_len;
    osal_u8 *data = OSAL_NULL;
    syschannel_hdr_stru *syschannel_hdr = OSAL_NULL;
    if (buf == OSAL_NULL) {
        return OSAL_NOK;
    }

    if (len <= ((osal_u32)hcc_get_head_len() + SYSCHANNEL_HDR_LEN)) {
        osal_kfree(buf);
        return OSAL_OK;
    }
    /* 去掉hcc头,获取syschannel头存的真实长度 */
    syschannel_hdr = (syschannel_hdr_stru *)(buf + hcc_get_head_len());
    data_len = syschannel_hdr->date_len;
    /* 转下字节序 */
    data_len = syschannel_ntohs(data_len);
    /* 再偏移掉syschannel头 */
    data = buf + hcc_get_head_len() + SYSCHANNEL_HDR_LEN;
    switch (stype) {
        case SYSCHANNEL_SUB_TYPE_MSG_MNT:
            if (syschannel_host_rx_msg_mnt(data, (osal_u32)data_len) != OSAL_OK) {
                osal_printk("syschannel_host_rx_msg_process:: syschannel_host_rx_msg_mnt failed");
            }
            break;
        case SYSCHANNEL_SUB_TYPE_MSG_APP:
            if (g_syschannel_handler_host[0].rx_func != OSAL_NULL) {
                if (g_syschannel_handler_host[0].rx_func(data, (int)data_len) != OSAL_OK) {
                    osal_printk("syschannel_host_rx_msg_process:: rx callback fail");
                }
            }
            break;
        default:
            osal_printk("syschannel_host_rx_msg_process:: unknown subtype.");
            break;
    }

    osal_kfree(buf);
    return OSAL_OK;
}

osal_void syschannel_host_awake_tcpip_tx_queue(hcc_queue_type queue_id)
{
    unref_param(queue_id);
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    if (g_queue_flow_flag == OSAL_TRUE) {
        oal_net_device_stru *netdev = oal_dev_get_by_name("wlan0");
        if (netdev == OSAL_NULL) {
            osal_printk("syschannel_host_awake_tcpip_tx_queue:: netdev is NULL");
            return;
        }
        oal_dev_put(netdev);
        oal_netif_wake_queue(netdev);
        g_queue_flow_flag = OSAL_FALSE;
    }
#endif
}
osal_void syschannel_host_stop_tcpip_tx_queue(hcc_queue_type queue_id)
{
    unref_param(queue_id);
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    if (g_queue_flow_flag == OSAL_FALSE) {
        oal_net_device_stru *netdev = oal_dev_get_by_name("wlan0");
        if (netdev == OSAL_NULL) {
            osal_printk("syschannel_host_stop_tcpip_tx_queue:: netdev is NULL");
            return;
        }
        oal_dev_put(netdev);
        oal_netif_stop_queue(netdev);
        g_queue_flow_flag = OSAL_TRUE;
    }
#endif
}

hcc_adapt_ops g_syschannel_host_data_adapt = {0};
hcc_adapt_ops g_syschannel_host_msg_adapt = {0};
osal_u32 syschannel_host_hcc_service_adapt_start(osal_u8 hcc_id, syschannel_service_type service_type,
    hcc_adapt_priv_rx_process rx_process)
{
    osal_u32 ret;
    hcc_adapt_ops *adapt = OSAL_NULL;

    if (service_type == SYSCHANNEL_SERVICE_TYPE_PKT) {
        adapt = &g_syschannel_host_data_adapt;
        adapt->alloc = syschannel_host_alloc_netbuf;
        adapt->free = syschannel_host_netbuf_free;
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
        adapt->start_subq = syschannel_host_awake_tcpip_tx_queue;
        adapt->stop_subq = syschannel_host_stop_tcpip_tx_queue;
#else
        adapt->start_subq = OSAL_NULL;
        adapt->stop_subq = OSAL_NULL;
#endif
    } else if (service_type == SYSCHANNEL_SERVICE_TYPE_MSG) {
        adapt = &g_syschannel_host_msg_adapt;
        adapt->alloc = syschannel_adapt_alloc_msg_buf;
        adapt->free = syschannel_adapt_free_msg_buf;
    } else {
        return OSAL_NOK;
    }
    adapt->rx_proc = rx_process;
    ret = hcc_service_init(hcc_id, service_type, adapt);
    if (ret != OSAL_OK) {
        osal_printk("host start hcc failed. type:%d, ret=0x%08x\r\n", service_type, ret);
    }
    osal_printk("host start hcc type:%d\r\n", service_type);
    return ret;
}

osal_u32 syschannel_host_wait_ready(osal_u8 hcc_id)
{
    osal_u32 retry_cnt = 0;
    /* 循环10次发送，每次等待50ms，直到通信稳定 */
    while (retry_cnt++ < BSP_READY_RETRY_TIMES) {
        g_bsp_ready = OSAL_FALSE;
        /* 发送host ready消息 */
        (osal_void)hcc_send_message(hcc_id, H2D_MSG_HOST_READY, 0);
        /* 等待device ready成功 */
        if (osal_wait_timeout_interruptible(&g_bsp_ready_wait, syschannel_bsp_ready_condition, NULL,
            BSP_READY_WAIT_MS) <= 0) {
            osal_printk("syschannel_host_init wait timeout\r\n");
        } else {
            break;
        }
    }
    return OSAL_OK;
}

static void syschannel_init_hcc_msg(osal_u8 hcc_id)
{
    hcc_message_register(hcc_id, 0, D2H_MSG_BSP_READY, syschannel_wait_bsp_ready, (osal_u8 *)&g_bsp_ready_wait);
    hcc_message_register(hcc_id, 0, D2H_MSG_HEARTBEAT, syschannel_d2h_headbeat, NULL);
    hcc_message_register(hcc_id, 0, D2H_MSG_SUSPEND_ACK, syschannel_wait_suspend_ack, (osal_u8 *)&g_suspend_ack_wait);
}

osal_u32 syschannel_host_create(osal_void)
{
    osal_u32 ret;
    osal_u8 hcc_id;
    hcc_channel_param channel_init; /* 初始化hcc通道 */
    syschannel_handler *syschannel_handler = syschannel_host_get_handler(); /* 判断是否已经通道初始化 */

    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_TRUE)) {
        osal_printk("syschannel_handler is used\r\n");
        return OSAL_NOK;
    }

#ifdef CONFIG_HCC_SUPPORT_SDIO
    channel_init.bus_type = HCC_BUS_SDIO;
    syschannel_handler->bus_type = SDIO_TYPE;
#elif defined(CONFIG_HCC_SUPPORT_SPI)
    channel_init.bus_type = HCC_BUS_SPI;
    syschannel_handler->bus_type = SPI_TYPE;
#endif
    channel_init.queue_cfg = syschannel_get_queue_cfg(&channel_init.queue_len);
    channel_init.unc_pool_size = SYSCHANNEL_HCC_UNC_POOL_SIZE;
    channel_init.unc_pool_low_limit = SYSCHANNEL_HCC_UNC_POOL_SIZE_LOW_LIMIT;
    channel_init.service_max_cnt = HCC_SERVICE_TYPE_MAX;
    channel_init.task_name = "syschannel";
    channel_init.task_pri = HOST_HCC_TASK_PRIORITY;

    hcc_id = hcc_init(&channel_init);
    if (hcc_id == HCC_CHANNEL_INVALID) {
        osal_printk("hcc init: fail\r\n");
        return OSAL_NOK;
    }

    syschannel_init_hcc_msg(hcc_id);
    hcc_set_tx_sched_count(hcc_id, HCC_TX_ASEEMBLE_CNT);
    /* 初始化hcc service */
    ret = syschannel_host_hcc_service_adapt_start(hcc_id, SYSCHANNEL_SERVICE_TYPE_PKT,
        syschannel_host_rx_data_process);
    if (ret != OSAL_OK) {
        return ret;
    }

    ret = syschannel_host_hcc_service_adapt_start(hcc_id, SYSCHANNEL_SERVICE_TYPE_MSG,
        syschannel_host_rx_msg_process);
    if (ret != OSAL_OK) {
        return ret;
    }

    syschannel_set_default_heartbeat_param(syschannel_handler);
    syschannel_handler->hcc_id = hcc_id;
    syschannel_handler->inuse = OSAL_TRUE;
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
    syschannel_handler->rx_func = oal_send_user_msg;
#else
    syschannel_handler->rx_func = OSAL_NULL;
#endif
    return syschannel_host_wait_ready(hcc_id);
}

STATIC osal_u32 syschannel_enable_heartbeat(osal_u8 enablehb)
{
    osal_u32 ret;
    heartbeat_ctrl_stru *heartbeat_ctrl = OSAL_NULL;
    osal_u8 buf[(SYSCHANNEL_MSG_TYPE_LEN + sizeof(heartbeat_ctrl_stru))];
    osal_u8 *buffer = OSAL_NULL;
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        osal_printk("syschannel_enable_heartbeat:: get syschannel_handler failed!\r\n");
        return OSAL_NOK;
    }

    /* 先按照消息类型进行组帧 */
    buffer = buf;
    buffer[0] = SYSCHANNEL_HEARTBEAT_ENABLE;
    buffer++;
    heartbeat_ctrl = (heartbeat_ctrl_stru *)(buffer);
    /* 根据入参进行使能或者去使能 */
    heartbeat_ctrl->enablehb = enablehb;
    heartbeat_ctrl->hb_timeout_threshold = syschannel_handler->hb_timeout_threshold;
    heartbeat_ctrl->hb_interval = syschannel_handler->hb_interval;

    ret = syschannel_tx_msg_adapt(syschannel_handler->hcc_id, buf, sizeof(buf), SYSCHANNEL_SERVICE_TYPE_MSG,
        SYSCHANNEL_SUB_TYPE_MSG_MNT);
    if (ret != OSAL_OK) {
        osal_printk("syschannel_enable_heartbeat tx failed\r\n");
        return ret;
    }
    syschannel_handler->enablehb = enablehb;
    return OSAL_OK;
}

/* 定时器超时函数 */
osal_u32 syschannel_heartbeat_timeout(osal_void)
{
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        osal_printk("syschannel_enable_heartbeat:: get syschannel_handler failed!\r\n");
        return OSAL_NOK;
    }

    g_heartbeat_host_cnt++;
    /* 重启收包定时器, osal_timer为单次定时器，需要手动重启 */
    syschannel_heartbeat_timer_restart();

    if (g_heartbeat_host_cnt >= syschannel_handler->hb_timeout_threshold) {
        syschannel_host_clear_heartbeat_count();
        /* 不删除超时定时器 */
        if (syschannel_handler->timeout_func != OSAL_NULL) {
            /* 调用注册函数 */
            syschannel_handler->timeout_func();
        }
        /* sdio状态恢复到探卡状态 todo */
        return OSAL_OK;
    }

    return OSAL_OK;
}

#if defined(SYSCHANNEL_SUPPORT_AUTO_RESUME) && (SYSCHANNEL_SUPPORT_AUTO_RESUME != 0)
static osal_s32 syschannel_host_resume_condition(const void *param)
{
    unref_param(param);
    return (g_resume_wake == OSAL_TRUE) || (osal_kthread_should_stop() != 0);
}

static int syschannel_host_resume_task(void *data)
{
    unref_param(data);
    osal_kthread_set_priority(g_resume_task, SYSCHANNEL_RESUME_TASK_PRIO);
    while (1) {
        if (osal_wait_interruptible(&g_resume_wait, syschannel_host_resume_condition, NULL) < 0) {
            osal_printk("%s INTERRUPTED!\r\n", __func__);
            break;
        }
        if (osal_kthread_should_stop() != 0) {
            osal_printk("%s exit!\r\n", __func__);
            break;
        }
        uapi_syschannel_host_resume(SYSCHANNEL_RESUME_MAX_RETRY, SYSCHANNEL_RESUME_MAX_DELAY);
        g_resume_wake = OSAL_FALSE;
    }
    osal_wait_destroy(&g_resume_wait);
    return EXT_ERR_SUCCESS;
}

static void syschannel_host_resume_exit(void)
{
    osal_kthread_destroy(g_resume_task, OSAL_TRUE);
}

static td_u32 syschannel_host_resume_init(void)
{
    if (osal_wait_init(&g_resume_wait) != OSAL_OK) {
        osal_printk("%s wait init fail!\r\n", __func__);
        return OSAL_NOK;
    }
    g_resume_wake = OSAL_FALSE;
    g_resume_task = osal_kthread_create((osal_kthread_handler)syschannel_host_resume_task, NULL,
        "syschannel_resume", SYSCHANNEL_RESUME_TASK_STACK);
    if (g_resume_task == TD_NULL) {
        osal_wait_destroy(&g_resume_wait);
        osal_printk("%s kthread init fail!\r\n", __func__);
        return OSAL_NOK;
    }
    return OSAL_OK;
}
#endif

static unsigned int syschannel_host_heartbeat_timeout_cb(void)
{
#if defined(SYSCHANNEL_SUPPORT_AUTO_RESUME) && (SYSCHANNEL_SUPPORT_AUTO_RESUME != 0)
    uapi_syschannel_host_do_suspend(false);
    g_resume_wake = OSAL_TRUE;
    osal_wait_wakeup(&g_resume_wait);
#endif
#ifdef CONFIG_SUPPORT_SLE_BASE_STATION
    /* 心跳超时 slave可能出现异常 重启 */
    hal_reboot_chip();
#endif
    return 0;
}

osal_void syschannel_heartbeat_timer_create(osal_void)
{
    osal_s32 ret;
    osal_u16 hb_interval;
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        osal_printk("syschannel_heartbeat_timer_create:: get syschannel_handler failed!");
        return;
    }

    syschannel_handler_register_timeout_cb(syschannel_handler, syschannel_host_heartbeat_timeout_cb);
    hb_interval = syschannel_handler->hb_interval;
    /* 创建并第一次启动定时器 */
    ret = osal_adapt_timer_init(&g_heartbeat_host_timer, syschannel_heartbeat_timeout, 0, hb_interval);
    if (ret != OSAL_OK) {
        osal_printk("syschannel_heartbeat_timer_create:: osal_adapt_timer_init failed!");
        (osal_void)osal_adapt_timer_destroy(&g_heartbeat_host_timer);
        g_heartbeat_host_timer.timer = NULL;
        return;
    }
    ret = osal_timer_start(&g_heartbeat_host_timer);
    if (ret != OSAL_OK) {
        osal_printk("syschannel_heartbeat_timer_create:: osal_timer_start failed!");
        (osal_void)osal_adapt_timer_destroy(&g_heartbeat_host_timer);
        g_heartbeat_host_timer.timer = NULL;
    }
}

osal_void syschannel_heartbeat_timer_destroy(osal_void)
{
    if (g_heartbeat_host_timer.timer == NULL) {
        return;
    }
    (osal_void)osal_adapt_timer_destroy(&g_heartbeat_host_timer);
    g_heartbeat_host_timer.timer = NULL;
}

osal_void syschannel_heartbeat_timer_restart(osal_void)
{
    osal_s32 ret;
    osal_u16 hb_interval;
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        osal_printk("syschannel_enable_heartbeat:: get syschannel_handler failed!");
        return;
    }
    hb_interval = syschannel_handler->hb_interval;
    ret = osal_adapt_timer_mod(&g_heartbeat_host_timer, hb_interval);
    if (ret != OSAL_OK) {
        osal_printk("syschannel_heartbeat_timer_restart:: osal_adapt_timer_mod failed!");
        (osal_void)osal_adapt_timer_destroy(&g_heartbeat_host_timer);
        g_heartbeat_host_timer.timer = NULL;
    }
}

/*****************************************************************************
 函 数 名  : syschannel_host_init
 功能描述  : syschannel host侧主初始化
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
osal_u32 syschannel_host_init(osal_void)
{
    osal_u32 ret;

    if (osal_wait_init(&g_bsp_ready_wait) != OSAL_OK) {
        osal_printk("syschannel_host_init ready wait init fail\r\n");
        return OSAL_NOK;
    }

    if (osal_wait_init(&g_suspend_ack_wait) != OSAL_OK) {
        osal_printk("syschannel_host_init suspend wait init fail\r\n");
        return OSAL_NOK;
    }

    /* 初始化hcc相关的结构体和服务 */
    ret = syschannel_host_create();
    if (ret != OSAL_OK) {
        osal_printk("host start hcc failed\r\n");
        return ret;
    }

    /* 发消息配置使能心跳检测 */
    ret = syschannel_enable_heartbeat(OSAL_TRUE);
    if (ret != OSAL_OK) {
        osal_printk("syschannel_enable_heartbeat failed\r\n");
    }

#if defined(SYSCHANNEL_SUPPORT_AUTO_RESUME) && (SYSCHANNEL_SUPPORT_AUTO_RESUME != 0)
    /* 恢复通信任务创建 */
    ret = syschannel_host_resume_init();
    if (ret != OSAL_OK) {
        osal_printk("syschannel_host_resume_init failed\r\n");
        return ret;
    }
#endif
    g_syschannel_status = SYSCHANNEL_INIT;
    return OSAL_OK;
}

osal_u32 syschannel_hcc_service_adapt_stop(osal_u8 hcc_id, hcc_service_type service_type)
{
    osal_u32 ret;

    ret = hcc_service_deinit(hcc_id, service_type);
    osal_printk("syschannel_hcc_service_adapt_stop type:%d, ret=%d\r\n", service_type, ret);
    return ret;
}

osal_void syschannel_host_hcc_exit(osal_void)
{
    /* 判断是否已经去初始化 */
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
    if ((syschannel_handler == OSAL_NULL) || (syschannel_handler->inuse == OSAL_FALSE)) {
        return;
    }
    osal_u8 hcc_id = syschannel_handler->hcc_id;
    hcc_message_unregister(hcc_id, 0, D2H_MSG_BSP_READY);
    hcc_message_unregister(hcc_id, 0, D2H_MSG_HEARTBEAT);
    osal_wait_destroy(&g_bsp_ready_wait);
    /* 卸载hcc service */
    syschannel_hcc_service_adapt_stop(hcc_id, SYSCHANNEL_SERVICE_TYPE_PKT);
    syschannel_hcc_service_adapt_stop(hcc_id, SYSCHANNEL_SERVICE_TYPE_MSG);
    hcc_deinit(hcc_id);
    syschannel_handler->inuse = OSAL_FALSE;
    syschannel_handler->rx_func = OSAL_NULL;
    syschannel_handler->timeout_func = OSAL_NULL;
}

/*****************************************************************************
 函 数 名  : syschannel_host_exit
 功能描述  : syschannel host侧主退出
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
osal_void syschannel_host_exit(osal_void)
{
    /* 发消息配置去使能心跳检测 */
    syschannel_enable_heartbeat(OSAL_FALSE);
    /* 关闭心跳检测定时器 */
    syschannel_heartbeat_timer_destroy();
    /* 卸载hcc相关所有接口 */
    syschannel_host_hcc_exit();
#if defined(SYSCHANNEL_SUPPORT_AUTO_RESUME) && (SYSCHANNEL_SUPPORT_AUTO_RESUME != 0)
    /* 卸载resume任务 */
    syschannel_host_resume_exit();
#endif
}

/*****************************************************************************
 函 数 名  : syschannel_host_reset
 功能描述  : host侧重启函数
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
osal_void syschannel_host_reset(osal_void)
{
    syschannel_host_clear_heartbeat_count();
    /* 如果定时器还在，则删除心跳检测定时器 */
    if (g_heartbeat_host_timer.timer != OSAL_NULL) {
        syschannel_heartbeat_timer_destroy();
    }
    syschannel_handler_register_timeout_cb(syschannel_host_get_handler(), OSAL_NULL);
    return;
}

#define SYSCHANNEL_RESUME_WAIT_MS       10
#define SYSCHANNEL_SUSPEND_ACK_WAIT_MS  2000
#define SYSCHANNEL_SUSPEND_TRY_CNT      3

static osal_u32 syschannel_wait_suspend_ack(osal_u8 *cb_data)
{
    g_suspend_ack = OSAL_TRUE;
    osal_wait_wakeup((osal_wait *)cb_data);
    return 0;
}

static osal_s32  syschannel_suspend_ack_condition(const void *param)
{
    unref_param(param);
    return g_suspend_ack;
}

static osal_u32 syschannel_host_wait_suspend_ack(osal_void)
{
    osal_u32 ret = EXT_ERR_SUCCESS;

    /* 等待device suspend ack */
    if (osal_wait_timeout_interruptible(&g_suspend_ack_wait, syschannel_suspend_ack_condition, NULL,
        SYSCHANNEL_SUSPEND_ACK_WAIT_MS) <= 0) {
        osal_printk("syschannel_host_wait_suspend_ack wait timeout\r\n");
        ret = EXT_ERR_FAILURE;
    }
    g_suspend_ack = OSAL_FALSE;
    return ret;
}

osal_u32 uapi_syschannel_host_do_suspend(osal_bool need_dev_reset)
{
    osal_u32 ret = EXT_ERR_FAILURE;
    osal_u32 try_cnt = 0;
    hcc_handler *hcc;

    syschannel_handler *syschannel_handler = syschannel_host_get_handler();
    if (syschannel_handler == OSAL_NULL) {
        return EXT_ERR_FAILURE;
    }
    hcc = hcc_get_handler(syschannel_handler->hcc_id);
    if ((hcc == OSAL_NULL) || (hcc->bus == OSAL_NULL)) {
        return EXT_ERR_FAILURE;
    }

    syschannel_host_reset();
    if (need_dev_reset) {
        // 防止消息丢失加尝试
        while ((ret == EXT_ERR_FAILURE) && (try_cnt++ < SYSCHANNEL_SUSPEND_TRY_CNT)) {
            hcc_send_message(syschannel_handler->hcc_id, H2D_MSG_HOST_SUSPEND, 0);
            ret = syschannel_host_wait_suspend_ack();
        }
        if (ret == EXT_ERR_SUCCESS) {
            hcc_bus_power_action(hcc->bus, HCC_BUS_POWER_DOWN);
        }
    }
    hcc_stop_xfer(syschannel_handler->hcc_id);
    g_syschannel_status = SYSCHANNEL_SUSPEND;
    return ret;
}

osal_u32 uapi_syschannel_host_suspend(osal_void)
{
    return uapi_syschannel_host_do_suspend(true);
}

osal_u32 uapi_syschannel_host_resume(osal_u32 try_cnt, osal_u32 delay_ms)
{
    osal_u32 i;
    osal_u32 ret = EXT_ERR_FAILURE;
    hcc_handler *hcc;
    syschannel_handler *syschannel_handler = syschannel_host_get_handler();

    if (syschannel_handler == OSAL_NULL || syschannel_handler->inuse == OSAL_FALSE) {
        return EXT_ERR_FAILURE;
    }
    hcc = hcc_get_handler(syschannel_handler->hcc_id);
    if ((hcc == OSAL_NULL) || (hcc->bus == OSAL_NULL)) {
        return EXT_ERR_FAILURE;
    }

    hcc_stop_xfer(syschannel_handler->hcc_id);
    for (i = 0; i < try_cnt; i++) {
        ret = (osal_u32)hcc_bus_reinit(hcc->bus);
        if (ret == EXT_ERR_SUCCESS) {
            break;
        }
        osal_msleep(delay_ms);
    }
    if (ret == EXT_ERR_SUCCESS) {
        hcc_resume_xfer(syschannel_handler->hcc_id);
        hcc_enable_switch(syschannel_handler->hcc_id, HCC_ON);
        /* 如何保证device已经退出syschannel */
        syschannel_host_wait_ready(syschannel_handler->hcc_id);
        syschannel_enable_heartbeat(OSAL_TRUE);
        g_syschannel_status = SYSCHANNEL_RESUME;
    }
    return ret;
}
#if defined(_PRE_OS_VERSION_LINUX) && defined(_PRE_OS_VERSION) && (_PRE_OS_VERSION == _PRE_OS_VERSION_LINUX)
EXPORT_SYMBOL(uapi_syschannel_host_suspend);
EXPORT_SYMBOL(uapi_syschannel_host_resume);
#endif

#define SYSCHANNEL_SUSPEND "syschannel_suspend"
#define SYSCHANNEL_RESUME  "syschannel_resume"

osal_u32 hcc_test_cmd_ext(const osal_s8 *buf, size_t count)
{
    osal_char *cmd_str = OSAL_NULL;

    unref_param(count);
    if ((cmd_str = osal_strstr(buf, SYSCHANNEL_SUSPEND)) != OSAL_NULL) {
        uapi_syschannel_host_suspend();
        return EXT_ERR_SUCCESS;
    } else if ((cmd_str = osal_strstr(buf, SYSCHANNEL_RESUME)) != OSAL_NULL) {
        osal_u32 try_cnt, delay_ms;
        osal_char *str_param = cmd_str + osal_strlen(SYSCHANNEL_RESUME);
        if (str_param != OSAL_NULL) {
            if (sscanf_s(str_param, "%d %d", &try_cnt, &delay_ms) == 0x2) {
                uapi_syschannel_host_resume(try_cnt, delay_ms);
            }
        }
        return EXT_ERR_SUCCESS;
    }
    return EXT_ERR_FAILURE;
}

#define SYSCHANNEL_SHOW_BUF_LEN  8
osal_u32 hcc_test_cmd_show_ext(const osal_s8 *buf)
{
    return snprintf_s((osal_s8 *)buf, SYSCHANNEL_SHOW_BUF_LEN, SYSCHANNEL_SHOW_BUF_LEN, "%d\r\n", g_syschannel_status);
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
