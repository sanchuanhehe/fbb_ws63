/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: W33 device info. \n
 *
 * History: \n
 * 2023-03-25, Create file. \n
 */

#include "w33_device_info.h"
#include "w33_board_port.h"
#include "tiot_pm_default.h"
#ifdef CONFIG_W33_UART
#include "tiot_board_uart_port.h"
#include "tiot_cfg_handle_priv.h"
#else
#include "tiot_board_spi_port.h"
#include "tiot_cfg_handle_xci.h"
#endif
#include "tiot_board_log.h"
#include "tiot_controller.h"
#include "tiot_controller_helper.h"
#include "tiot_sys_msg_handle.h"

#define W33_TXCO_FREQ            ((uint32_t)CONFIG_W33_TCXO_FREQ)

#define W33_POWERON_WAIT_MS      1      /* Wait time before power on. */
#define W33_BOOT_TIME_MS         20     /* Power on ==> Boot time. */
#define W33_INIT_TIME_MS         15     /* Boot ==> Init time. */
#define W33_WAKEUP_PULSE_MS      1      /* GPIO wake up pulse. */
#define W33_WAKEUP_WAIT_MS       50     /* Wake up wait ms. */

#ifdef CONFIG_W33_UART
#define W33_BAUD_CHANGE_WAIT_US  11000  /* UART切换波特率等待时间. Baudrate change wait μs. */

#ifdef CONFIG_W33_UART_WITH_FLOWCTRL
#define W33_UART_ATTR_FLOW_CTRL       TIOT_UART_ATTR_FLOW_CTRL_ENABLE
#else
#define W33_UART_ATTR_FLOW_CTRL       TIOT_UART_ATTR_FLOW_CTRL_DISABLE
#endif

#define W33_UART_FW_LOAD_BAUDRATE    (W33_TXCO_FREQ / 20)   /* 固件加载默认波特率，20分频. */
#define W33_UART_DEFAULT_BAUDRATE     2000000U              /* 数据收发默认波特率. */

#else
#define W33_BAUD_CHANGE_WAIT_US  0      /* SPI 无切换波特率等待时间. */
#endif

static const tiot_pm_event_entry g_w33_pm_event_map[TIOT_PM_EVENT_MAX] = {
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_POWEROFF, tiot_pm_default_power_off },
    /* 上电后等待init消息或业务消息才说明进入work状态 */
    { TIOT_PM_TAG_CAN_POWEROFF, TIOT_PM_STATE_INIT, tiot_pm_default_power_on },
    /* 只有sleep态才需要唤醒 */
#ifdef CONFIG_W33_WAKEUP_TYPE_UART
    { TIOT_PM_TAG_CAN_WAKEUP, TIOT_PM_STATE_WAKING, tiot_pm_default_wakeup_device_by_uart },
#else
    { TIOT_PM_TAG_CAN_WAKEUP, TIOT_PM_STATE_WAKING, tiot_pm_default_wakeup_device_by_gpio },
#endif
    /* 有device wakeup host管脚时，通过此管脚说明已唤醒，且需要下发disallow消息 */
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_WORK, tiot_pm_default_wakeup_ack_handle },
    /* 接收到request sleep消息，下发allow sleep消息 */
    { TIOT_PM_TAG_CAN_ALLOW_SLEEP, TIOT_PM_STATE_SLEEPING, tiot_pm_default_request_sleep_handle },
    /* 接收到dev wkup host gpio拉低，需要释放唤醒锁 */
    { TIOT_PM_TAG_CAN_SLEEP, TIOT_PM_STATE_SLEEP, tiot_pm_default_sleep_ack_handle },
    /* 非S3状态，下发allow sleep并释放唤醒锁 */
    { TIOT_PM_TAG_CAN_ALLOW_SLEEP, TIOT_PM_STATE_SLEEP, tiot_pm_default_request_sleep_ack_handle },
    /* 上报init消息或业务消息说明唤醒成功 */
    { TIOT_PM_TAG_CAN_WORK, TIOT_PM_STATE_WORK, tiot_pm_default_report_work },
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_INVALID, tiot_pm_default_work_vote_up },
    { TIOT_PM_TAG_CAN_POWERON, TIOT_PM_STATE_INVALID, tiot_pm_default_work_vote_down }
};

static void w33_firmware_before_load(tiot_fw *fw)
{
#ifdef CONFIG_W33_UART
    /* 加载前设置为固件加载默认波特率 */
    tiot_xmit_config w33_fw_load_config = def_uart_cfg(W33_UART_FW_LOAD_BAUDRATE,
                                                       W33_UART_ATTR_FLOW_CTRL);
    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    tiot_xfer_manager *xfer = &ctrl->transfer;
    if (tiot_xfer_set_config(xfer, &w33_fw_load_config) != ERRCODE_TIOT_SUCC) {
        tiot_print_dbg("[TIoT][w33]config before load fail.\r\n");
    }
    /* 加载前先清掉rx_buff */
#ifdef CONFIG_XFER_DEFAULT_RX_BUFF
    if (xfer->xmit_ops->rx_mode == TIOT_XMIT_RX_MODE_BUFFED) {
        circ_buf_flush(&xfer->rx_buff);
    }
#endif
#else
    // SPI 不需要切频率
    tiot_unused(fw);
#endif
}

static void w33_firmware_after_load(tiot_fw *fw)
{
#ifdef CONFIG_W33_UART
    /* 加载过程中可能切换波特率，加载后设置为业务默认波特率 */
    tiot_xmit_config w33_transfer_config = def_uart_cfg(W33_UART_DEFAULT_BAUDRATE,
                                                        W33_UART_ATTR_FLOW_CTRL);
    tiot_controller *ctrl = tiot_container_of(fw, tiot_controller, firmware);
    tiot_xfer_manager *xfer = &ctrl->transfer;
    if (tiot_xfer_set_config(xfer, &w33_transfer_config) != ERRCODE_TIOT_SUCC) {
        tiot_print_dbg("[TIoT][w33]config after load fail.\r\n");
    }
#else
    // SPI 不需要切频率
    tiot_unused(fw);
#endif
}

static int w33_rx_wait_func(const void *param)
{
#ifdef CONFIG_W33_UART
    /* 通过UART上报数据，不需要拉高管脚来通知读取 */
    tiot_unused(param);
    return 0;
#else
    uint8_t pin_level;
    tiot_xfer_manager *xfer = (tiot_xfer_manager *)param;
    tiot_controller *controller = tiot_container_of(xfer, tiot_controller, transfer);
    uint32_t wakein_pin = controller->pm.pm_info[TIOT_PIN_WAKE_IN];
    tiot_board_pin_get_level(wakein_pin, &pin_level);
    return ((pin_level == TIOT_PIN_LEVEL_HIGH) ? (1) : 0);
#endif
}

static const tiot_device_info g_w33_device_info = {
    .pm_event_map = (tiot_pm_event_entry *)g_w33_pm_event_map,
    .pm_event_map_size = sizeof(g_w33_pm_event_map) / sizeof(tiot_pm_event_entry),
#ifdef CONFIG_W33_UART
    .exec_cbs = { w33_firmware_before_load, w33_firmware_after_load, tiot_fw_ext_cmd_handle_priv,
                  tiot_fw_pre_ext_cmd_handle_priv },
#else
    .exec_cbs = { w33_firmware_before_load, w33_firmware_after_load, tiot_fw_ext_cmd_handle_xci,
                  tiot_fw_pre_ext_cmd_handle_xci },
#endif
#ifdef CONFIG_W33_UART
    .xfer_info = { TIOT_XMIT_TYPE_UART, w33_rx_wait_func },
#else
    .xfer_info = { TIOT_XMIT_TYPE_SPI, w33_rx_wait_func },
#endif
    .pkt_sys_msg_handle = tiot_pkt_sys_msg_handle,
    .timings = {
        .power_on_wait_ms = W33_POWERON_WAIT_MS,
        .boot_time_ms = W33_BOOT_TIME_MS,
        .init_time_ms = W33_INIT_TIME_MS,
        .wakeup_pulse_ms = W33_WAKEUP_PULSE_MS,
        .wakeup_wait_ms = W33_WAKEUP_WAIT_MS,
        .baud_change_wait_us = W33_BAUD_CHANGE_WAIT_US,
    }
};

const tiot_device_info *w33_device_get_info(void)
{
    return &g_w33_device_info;
}
