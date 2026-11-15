#ifndef WS63_UART_H__
#define WS63_UART_H__
 
#include "pinctrl.h"
#include "driver/uart.h"
#include "osal_event.h"
#include "soc_osal.h"
#define UART0_TX_PIN 17
#define UART0_RX_PIN 18
#define BAUD_RATE 115200
#define BUFFER_SIZE 200

#define UART0_RX_EVENT_READ 0x01

extern uint8_t g_app_uart_rx_buff[1];
extern uint8_t g_app_uart_int_rx_buff[BUFFER_SIZE];
extern osal_event g_app_uart_event;
extern volatile uint8_t g_app_uart_rx_flag;
extern uint8_t g_app_uart_int_rx_buff_index;

void app_uart_read_int_handler(const void *buffer, uint16_t length, bool error);
void app_uart_register_rx_callback(uart_bus_t bus);
void init_uart(uart_bus_t bus,pin_t tx_pin, pin_t rx_pin, uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity);


#endif