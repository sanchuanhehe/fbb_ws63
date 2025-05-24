/**
 * Copyright (c) Adragon
 *
 * Description: WS63 Uart receiving and transmitting case based on interrupt mode . \n
 *              This case also includes a set of AT transceiver frameworks.
 *
 *
 * History: \n
 * 2025-5-20, Create file. \n
 */
#include <ws63_uart.h>

#define GT_TACK_STACK_SIZE 0x2000
#define GT_TACK_PRIO 24
#define UART_REC_TACK_STACK_SIZE 0x2000
#define UART_REC_TASK_PRIO 20
#define UART_PROGRESS_TACK_STACK_SIZE 0x2000
#define UART_PROGRESS_TASK_PRIO 24

extern void UartInt_Send_Cmd(uint8_t *cmd, uint8_t *expect_rbk, uint8_t sent_react_times);
extern void Uart_Rec_Task();
extern void Uart_Progress_Task();

extern uint8_t g_app_uart_rx_flag;
extern WS63Uart_Rec_State_t WS63Uart_Rec_State;

static void *Gateway_Task(void)
{
    osal_kthread_lock();
    osal_task *Uart_Rec_Task_handle = NULL;
    Uart_Rec_Task_handle = osal_kthread_create((osal_kthread_handler)Uart_Rec_Task, 0, "Uart_Rec_Task", UART_REC_TACK_STACK_SIZE);
    if (Uart_Rec_Task_handle != NULL)
    {
        osal_kthread_set_priority(Uart_Rec_Task_handle, UART_REC_TASK_PRIO);
    }

    osal_task *Uart_Progress_Task_handle = NULL;
    Uart_Progress_Task_handle = osal_kthread_create((osal_kthread_handler)Uart_Progress_Task, 0, "Uart_Progress_Task", UART_PROGRESS_TACK_STACK_SIZE);
    if (Uart_Progress_Task_handle != NULL)
    {
        osal_kthread_set_priority(Uart_Progress_Task_handle, UART_PROGRESS_TASK_PRIO);
    }
    osal_kthread_unlock();

    // 等待4G初始化(10s)
    uint32_t timeout = 10000;                   // 定义超时时间，单位为毫秒
    uint32_t start_time = uapi_tcxo_get_ms();   // 获取当前时间戳
    while (WS63Uart_Rec_State.L610_Init_Flag != 1)
    {
        osal_msleep(5);
        uapi_watchdog_kick();
        uint32_t current_time = uapi_tcxo_get_ms(); // 获取当前时间戳
        if ((current_time - start_time) >= timeout) // 检查是否超时
        {
            osal_printk("4G Init Timeout occurred!\r\n");
            g_app_uart_rx_flag = 0;
            break;
        }
    }

    // MQTT入网
    UartInt_Send_Cmd("ATE0\r\n", "OK", 0);
    UartInt_Send_Cmd("AT+MIPCALL=1\r\n", "OK", 1);
    osal_msleep(1000);
    UartInt_Send_Cmd("AT+MQTTUSER=1,\"username\",\"userid\",\"password\"\r\n", "OK", 1);
    UartInt_Send_Cmd("AT+MQTTOPEN=1,\"xxx.xxx.xx.xxx\",port,0,60\r\n", "+MQTTOPEN", 0);
    UartInt_Send_Cmd("AT+MQTTSUB=1,\"v1/devices/me/rpc/request/+\",1\r\n", "OK", 0);
    osal_msleep(1000);
    UartInt_Send_Cmd("AT+MQTTSUB=1,\"v1/devices/me/rpc/request/+\",1\r\n", "OK", 0);
    UartInt_Send_Cmd("AT+MQTTSUB=1,\"v1/devices/me/attributes/response/+\",1\r\n","OK", 0);
    UartInt_Send_Cmd("AT+MQTTSUB=1,\"v1/devices/me/attributes\",1\r\n", "OK", 0);
    osal_printk("*******************************************\r\n");
    osal_printk("Uart_MQTT_Test Init Done!\r\n");
    osal_printk("*******************************************\r\n");
    osal_msleep(500);
}

void TaskEntry(void)
{
    osal_task *GT_task_handle = NULL;

    osal_kthread_lock();
    GT_task_handle = osal_kthread_create((osal_kthread_handler)Gateway_Task, 0, "Gateway_Task", GT_TACK_STACK_SIZE);
    if (GT_task_handle != NULL)
    {
        osal_kthread_set_priority(GT_task_handle, UART_PROGRESS_TASK_PRIO);
    }
    osal_kthread_unlock();
}

app_run(TaskEntry);