/**
 * Copyright (h) Adragon
 *
 * Description: WS63 Uart receiving and transmitting case based on interrupt mode . \n
 *              This case also includes a set of AT transceiver frameworks.
 *
 *
 * History: \n
 * 2025-5-20, Create file. \n
 */
#ifndef WS63_UART_H
#define WS63_UART_H

#include "securec.h"
#include "osal_task.h"
#include "osal_event.h"
#include "osal_debug.h"
#include "app_init.h"
#include "soc_osal.h"
#include "osal_semaphore.h"
#include "watchdog.h"
#include "pinctrl.h"
#include "driver/uart.h"
#include "chip_core_irq.h"
#include "gpio.h"
#include <errcode.h>

// UART中断发送传参结构体 //
typedef struct 
{
    uint8_t *cmd;
    uint8_t *expect_rbk;
    uint8_t send_react_times;
}uart_write_int_param_t;

// UART接收状态结构体 //
typedef struct
{
    // 4G模块相关参数 //
    uint8_t L610_Init_Flag;     // L610初始化标志

    // MQTT相关参数 //
    uint8_t mqtt_register_finish_flag; // MQTT注册完成标志位
    uint8_t mqtt_break_flag;           // MQTT断开重连标志位

    // HTTP相关参数 //
    int8_t http_post_state;     // HTTP POST状态

    // OTA升级相关参数 //
    uint8_t upg_flag;           // OTA升级标志位
    // uint8_t upg_flag_ready;     // OTA升级准备完成标志位
    uint32_t file_size;         // 固件总大小
    uint8_t OTA_Version[20];    // 固件版本号
    uint32_t upg_offset;        // 固件偏移量
    int16_t chunk_num;          // 固件分块编号(用于记录已接收到的分块编号)
    int16_t get_chunk_num;      // 固件分块编号(用于记录当前接收的分块编号)
    int16_t last_chunk_num;     // 固件最后一块编号
    
}WS63Uart_Rec_State_t;


void UartInt_Send_Cmd(uint8_t *cmd, uint8_t *expect_rbk, uint8_t sent_react_times);
void Uart_Rec_Task();
void Uart_Progress_Task();

extern void UartInt_Send_Cmd(uint8_t *cmd, uint8_t *expect_rbk, uint8_t sent_react_times);
extern void Uart_Rec_Task();
extern void Uart_Progress_Task();

extern uint8_t g_app_uart_rx_flag;
extern WS63Uart_Rec_State_t WS63Uart_Rec_State;

#endif // WS63_UART_H