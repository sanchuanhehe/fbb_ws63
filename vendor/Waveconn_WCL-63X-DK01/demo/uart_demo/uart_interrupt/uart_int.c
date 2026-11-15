#include "ws63_uart.h"

#include "app_init.h"   
#include "common_def.h"  
#include "soc_osal.h"    
#include "securec.h"    
#include "stdio.h"
#include "cJSON.h"

static void *uart_thread(void)
{
    init_uart(UART_BUS_0, UART0_TX_PIN, UART0_RX_PIN, BAUD_RATE, UART_DATA_BIT_8, UART_STOP_BIT_1, UART_PARITY_NONE);
 
    while (1)
    {
        if (osal_event_read(&g_app_uart_event, UART0_RX_EVENT_READ, OSAL_WAIT_FOREVER, OSAL_WAITMODE_AND) != OSAL_FAILURE)
        {
        loop:
            osal_msleep(5);
            if (osal_event_read(&g_app_uart_event, UART0_RX_EVENT_READ, OSAL_WAIT_CONDITION_TRUE, OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR) != OSAL_FAILURE)
            {
                goto loop;
            }
            else
            {
                osal_printk("g_app_uart_int_rx_buff:%s\r\n", g_app_uart_int_rx_buff);
                memset(g_app_uart_int_rx_buff, 0, BUFFER_SIZE);
                g_app_uart_int_rx_buff_index = 0;
            }
        }
    }
    return NULL;
}

static void uart_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)uart_thread, 0, "UartTask", 0x2000);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, 25);
    }
    osal_kthread_unlock();
}

/* Run the uart_entry. */
app_run(uart_entry);