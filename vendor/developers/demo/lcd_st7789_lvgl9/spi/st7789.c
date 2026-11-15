/**
 * Copyright (c) EndyTsang 2025. All rights reserved. \n
 *
 * Description: LVGL9 DEMO. \n
 *
 */

#include "st7789.h"

void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    {
        uint8_t data[] = {x0 >> HIGH_BYTE_SHIFT, x0 & LOW_BYTE_MASK, x1 >> HIGH_BYTE_SHIFT, x1 & LOW_BYTE_MASK};
        send_cmd(ST7789_CASET);
        send_data_array(data, sizeof(data));
    }

    {
        uint8_t data[] = {y0 >> HIGH_BYTE_SHIFT, y0 & LOW_BYTE_MASK, y1 >> HIGH_BYTE_SHIFT, y1 & LOW_BYTE_MASK};
        send_cmd(ST7789_RASET);
        send_data_array(data, sizeof(data));
    }
}

void st7789_write_data(uint8_t *data, uint32_t len)
{
    send_cmd(ST7789_RAMWR);
    send_data_array(data, len);
}

void st7789_init(void)
{
    spi_init();
    osal_printk("spi init\r\n");

    uapi_gpio_set_val(CONFIG_SPI_RESET_MASTER_PIN, GPIO_LEVEL_LOW);
    osal_msleep(100); // 延时100毫秒
    uapi_gpio_set_val(CONFIG_SPI_RESET_MASTER_PIN, GPIO_LEVEL_HIGH);
    osal_msleep(100); // 延时100毫秒

    send_cmd(ST7789_SLPOUT);

    send_cmd(ST7789_COLMOD);
    send_data(0x55); //   5 565  6 666

    send_cmd(0xB2);
    send_data(0x0C);
    send_data(0x0C);
    send_data(0x00);
    send_data(0x33);
    send_data(0x33);

    send_cmd(0xB7);
    send_data(0x35);

    send_cmd(0xBB);
    send_data(0x32); // Vcom=1.35V

    send_cmd(0xC2);
    send_data(0x01);

    send_cmd(0xC3);
    send_data(0x19); // GVDD=4.8V

    send_cmd(0xC4);
    send_data(0x20); // VDV, 0x20:0v

    send_cmd(0xC6);
    send_data(0x0F); // 0x0F:60Hz

    send_cmd(0xD0);
    send_data(0xA4);
    send_data(0xA1);

    send_cmd(0xE0);
    send_data(0xD0);
    send_data(0x08);
    send_data(0x0E);
    send_data(0x09);
    send_data(0x09);
    send_data(0x05);
    send_data(0x31);
    send_data(0x33);
    send_data(0x48);
    send_data(0x17);
    send_data(0x14);
    send_data(0x15);
    send_data(0x31);
    send_data(0x34);

    send_cmd(0xE1);
    send_data(0xD0);
    send_data(0x08);
    send_data(0x0E);
    send_data(0x09);
    send_data(0x09);
    send_data(0x15);
    send_data(0x31);
    send_data(0x33);
    send_data(0x48);
    send_data(0x17);
    send_data(0x14);
    send_data(0x15);
    send_data(0x31);
    send_data(0x34);

    send_cmd(ST7789_INVON);

    send_cmd(ST7789_MADCTL); // MX, MY, RGB mode

    send_data(0x00);

    send_cmd(ST7789_DISPON);
}
