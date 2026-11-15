#include "ws63_uart.h"

uint8_t g_app_uart_rx_buff[1] = {0};
uint8_t g_app_uart_int_rx_buff[BUFFER_SIZE] = {0};
volatile uint8_t g_app_uart_rx_flag = 0;
osal_event g_app_uart_event = {0};
uint8_t g_app_uart_int_rx_buff_index = 0;


void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error)
{
    unused(error);
    unused(buffer);
    if (buffer == NULL || length == 0)
    {
        osal_printk("uart%d int mode transfer illegal data!\r\n", UART_BUS_0);
        return;
    }
    uint8_t *buff = (uint8_t *)buffer;
    g_app_uart_int_rx_buff[g_app_uart_int_rx_buff_index++] = buff[0];
    if (g_app_uart_int_rx_buff_index >= BUFFER_SIZE)
    {
        g_app_uart_int_rx_buff_index = 0;
    }
    
    // osal_printk("index: %d,\r\n", g_app_uart_int_rx_buff_index);
    osal_event_write(&g_app_uart_event, UART0_RX_EVENT_READ);
    g_app_uart_rx_flag = 1;
}

void app_uart_register_rx_callback(uart_bus_t bus)
{
    osal_printk("uart%d int mode register receive callback start!\r\n", bus);
    if (uapi_uart_register_rx_callback(bus, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE, 1, app_uart_read_int_handler) == ERRCODE_SUCC)
    {
        osal_printk("uart%d int mode register receive callback succ!\r\n", bus);
    }
}

void app_uart_event_init(void)
{
    if(osal_event_init(&g_app_uart_event) != OSAL_SUCCESS)
    {
        osal_printk("uart event init failed\r\n");
    }
    else
    {
        osal_printk("uart event init success\r\n");
    }
}

void init_uart(uart_bus_t bus,pin_t tx_pin, pin_t rx_pin, uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity)
{
    uapi_uart_deinit(bus);
    uapi_pin_set_mode(tx_pin, PIN_MODE_1);
    uapi_pin_set_mode(rx_pin, PIN_MODE_1);
    uart_attr_t attr =
        {
            .baud_rate = baud_rate,
            .data_bits = data_bits,
            .stop_bits = stop_bits,
            .parity = parity};
    uart_pin_config_t pin_config =
        {
            .tx_pin = tx_pin,
            .rx_pin = rx_pin,
            .cts_pin = PIN_NONE,
            .rts_pin = PIN_NONE};
    uart_buffer_config_t g_app_uart_buffer_config = {
        .rx_buffer = g_app_uart_rx_buff,
        .rx_buffer_size = 1};
    uapi_uart_init(bus, &pin_config, &attr, NULL, &g_app_uart_buffer_config);
    app_uart_register_rx_callback(bus);
    app_uart_event_init();
}