#include "lcd.h"
#include "pinctrl.h"
#include "watchdog.h"
#include "gpio.h"
#include "spi.h"
#include "soc_osal.h"
#include "dma.h"

#define SPI_SLAVE_NUM       1
#define SPI_BUS_CLK         96000000
#define SPI_FREQ_MHZ        48
#define SPI_CLK_POLARITY    0
#define SPI_CLK_PHASE       0
#define SPI_FRAME_FORMAT    0
#define SPI_FRAME_FORMAT_STANDARD   0
#define SPI_TMOD            0
#define SPI_WAIT_CYCLES     0x10
#define CONFIG_SPI_TRANSFER_LEN     1

lcd_dev_t g_lcd_dev;
uint8_t g_lcd_buf[LCD_BUF_MAX] = {0};
uint8_t g_lcd_buf2[LCD_BUF_MAX] = {0};

void lcd_wr_command(uint8_t data)
{
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(CONFIG_SPI_RS_PIN, GPIO_LEVEL_LOW);
    spi_xfer_data_t xfer = {
        .tx_buff = &data,
        .tx_bytes = CONFIG_SPI_TRANSFER_LEN,
    };
    errcode_t ret = uapi_spi_master_write(SPI_BUS_0, &xfer, 0xFFFFFFFF);
    if (ret != ERRCODE_SUCC) {
        printf("%d SPI send failed: 0x%x\n", __LINE__, ret);
    }
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_HIGH);
}

void lcd_wr_data(uint8_t data)
{
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(CONFIG_SPI_RS_PIN, GPIO_LEVEL_HIGH);
    spi_xfer_data_t xfer = {
        .tx_buff = &data,
        .tx_bytes = CONFIG_SPI_TRANSFER_LEN,
    };
    errcode_t ret = uapi_spi_master_write(SPI_BUS_0, &xfer, 0xFFFFFFFF);
    if (ret != ERRCODE_SUCC) {
        printf("%d SPI send failed: 0x%x\n", __LINE__, ret);
    }
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_HIGH);
}

void lcd_diplay(uint8_t *data, uint32_t pixel_len)
{
    if (data == NULL || pixel_len == 0) {
        printf("Input invalid.\r\n");
        return;
    }
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(CONFIG_SPI_RS_PIN, GPIO_LEVEL_HIGH);

    uint8_t buf_count = (pixel_len % DMA_SIZE == 0) ? pixel_len / DMA_SIZE : pixel_len / DMA_SIZE + 1;
    spi_xfer_data_t xfer = {0};
    errcode_t ret;

    for (uint8_t i = 0; i < buf_count; i++) {
        xfer.tx_buff = data + i * DMA_SIZE;
        xfer.tx_bytes = (i != buf_count - 1)? DMA_SIZE: pixel_len % DMA_SIZE;
        ret = uapi_spi_master_write(SPI_BUS_0, &xfer, 0xFFFFFFFF);
        if (ret != ERRCODE_SUCC) {
            printf("%d SPI send failed: 0x%x\n", __LINE__, ret);
        }
    }

    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_HIGH);
}

void lcd_wr_reg(uint8_t lcd_reg, uint16_t lcd_reg_value)
{
    lcd_wr_command(lcd_reg);
    lcd_wr_data(lcd_reg_value);
}

void lcd_write_ram_prepare(void)
{
    lcd_wr_command(g_lcd_dev.wramcmd);
}

void lcd_clear(uint16_t color)
{
    unsigned int i;
    unsigned int m;
    lcd_set_windows(0, 0, g_lcd_dev.width - 1, g_lcd_dev.height - 1);
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_LOW);
    uapi_gpio_set_val(CONFIG_SPI_RS_PIN, GPIO_LEVEL_HIGH);
    for (i = 0; i < g_lcd_dev.height; i++) {
        for (m = 0; m < g_lcd_dev.width; m++) {
            lcd_wr_data(color >> 8);
            lcd_wr_data(color);
        }
    }
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_HIGH);
}

void app_spi_gpio_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_SPI_DI_PIN, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_DO_PIN, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_CLK_PIN, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_SPI_CS_PIN, CONFIG_GPIO_PIN_MODE);
    uapi_gpio_set_dir(CONFIG_SPI_CS_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_SPI_CS_PIN, GPIO_LEVEL_HIGH);
    uapi_pin_set_mode(CONFIG_SPI_RS_PIN, CONFIG_GPIO_PIN_MODE);
    uapi_gpio_set_dir(CONFIG_SPI_RS_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_SPI_RS_PIN, GPIO_LEVEL_HIGH);
    uapi_pin_set_mode(CONFIG_SPI_RST_PIN, CONFIG_GPIO_PIN_MODE);
    uapi_gpio_set_dir(CONFIG_SPI_RST_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_SPI_RST_PIN, GPIO_LEVEL_HIGH);
    uapi_pin_set_mode(CONFIG_SPI_BLK_PIN, CONFIG_GPIO_PIN_MODE);
    uapi_gpio_set_dir(CONFIG_SPI_BLK_PIN, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_SPI_BLK_PIN, GPIO_LEVEL_HIGH);
}

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
static void app_spi_master_write_int_handler(const void *buffer, uint32_t length)
{
    unused(buffer);
    unused(length);
    osal_printk("spi master write length = %d!\r\n", length);
}
#endif

void spi_init(void)
{
    spi_attr_t config = {0};
    spi_extra_attr_t ext_config = {0};

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = SPI_BUS_CLK;
    config.freq_mhz = SPI_FREQ_MHZ;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.tmod = SPI_TMOD;
    config.sste = 0;

    ext_config.tx_use_dma = true;
    ext_config.sspi_param.wait_cycles = SPI_WAIT_CYCLES;
    osal_printk("SPI freq = %dMHz\r\n", config.freq_mhz);
    int ret = uapi_spi_init(SPI_BUS_0, &config, &ext_config);
    if (ret != 0) {
        osal_printk("spi init fail %0x\r\n", ret);
    }

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    uapi_dma_init();
    uapi_dma_open();
#ifndef CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH
    spi_dma_config_t dma_cfg = {
        .src_width = 0,
        .dest_width = 0,
        .burst_length = 0,
        .priority = 0
    };
    if (uapi_spi_set_dma_mode(SPI_BUS_0, true, &dma_cfg) != ERRCODE_SUCC) {
        osal_printk("spi%d master set dma mode fail!\r\n");
    }
#endif
#endif

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (uapi_spi_set_irq_mode(SPI_BUS_0, true, NULL,
        app_spi_master_write_int_handler) == ERRCODE_SUCC) {
        osal_printk("spi%d master set irq mode succ!\r\n", SPI_BUS_0);
    } else {
        osal_printk("spi%d master set irq mode failed!\r\n", SPI_BUS_0);
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
}

void lcd_reset(void)
{
    uapi_gpio_set_val(CONFIG_SPI_RST_PIN, GPIO_LEVEL_LOW);
    osal_mdelay(100);
    uapi_gpio_set_val(CONFIG_SPI_RST_PIN, GPIO_LEVEL_HIGH);
    osal_mdelay(50);
}

void lcd_init(void)
{
    uapi_watchdog_disable();
    app_spi_gpio_init_pin();
    spi_init();
    lcd_reset();
    //*************3.5 ST7796S IPS**********//
    lcd_wr_command(0x11);

    osal_mdelay(120); // Delay 120ms

    lcd_wr_command(0x36); // Memory Data Access Control MY,MX~~
    lcd_wr_data(0x48);

    lcd_wr_command(0x3A);
    lcd_wr_data(0x55);

    lcd_wr_command(0xF0); // Command Set Control
    lcd_wr_data(0xC3);

    lcd_wr_command(0xF0);
    lcd_wr_data(0x96);

    lcd_wr_command(0xB4);
    lcd_wr_data(0x02);

    lcd_wr_command(0xB7);
    lcd_wr_data(0xC6);

    lcd_wr_command(0xC0);
    lcd_wr_data(0xC0);
    lcd_wr_data(0x00);

    lcd_wr_command(0xC1);
    lcd_wr_data(0x13);

    lcd_wr_command(0xC2);
    lcd_wr_data(0xA7);

    lcd_wr_command(0xC5);
    lcd_wr_data(0x21);

    lcd_wr_command(0xE8);
    lcd_wr_data(0x40);
    lcd_wr_data(0x8A);
    lcd_wr_data(0x1B);
    lcd_wr_data(0x1B);
    lcd_wr_data(0x23);
    lcd_wr_data(0x0A);
    lcd_wr_data(0xAC);
    lcd_wr_data(0x33);

    lcd_wr_command(0xE0);
    lcd_wr_data(0xD2);
    lcd_wr_data(0x05);
    lcd_wr_data(0x08);
    lcd_wr_data(0x06);
    lcd_wr_data(0x05);
    lcd_wr_data(0x02);
    lcd_wr_data(0x2A);
    lcd_wr_data(0x44);
    lcd_wr_data(0x46);
    lcd_wr_data(0x39);
    lcd_wr_data(0x15);
    lcd_wr_data(0x15);
    lcd_wr_data(0x2D);
    lcd_wr_data(0x32);

    lcd_wr_command(0xE1);
    lcd_wr_data(0x96);
    lcd_wr_data(0x08);
    lcd_wr_data(0x0C);
    lcd_wr_data(0x09);
    lcd_wr_data(0x09);
    lcd_wr_data(0x25);
    lcd_wr_data(0x2E);
    lcd_wr_data(0x43);
    lcd_wr_data(0x42);
    lcd_wr_data(0x35);
    lcd_wr_data(0x11);
    lcd_wr_data(0x11);
    lcd_wr_data(0x28);
    lcd_wr_data(0x2E);

    lcd_wr_command(0xF0);
    lcd_wr_data(0x3C);
    lcd_wr_command(0xF0);
    lcd_wr_data(0x69);

    osal_mdelay(120);
    lcd_wr_command(0x21);
    lcd_wr_command(0x29);

    lcd_direction(USE_HORIZONTAL);
    lcd_clear(WHITE);
}

void lcd_set_windows(uint16_t x_star, uint16_t y_star, uint16_t x_end, uint16_t y_end)
{
    lcd_wr_command(g_lcd_dev.setxcmd);
    lcd_wr_data(x_star >> 8);
    lcd_wr_data(0x00FF & x_star);
    lcd_wr_data(x_end >> 8);
    lcd_wr_data(0x00FF & x_end);

    lcd_wr_command(g_lcd_dev.setycmd);
    lcd_wr_data(y_star >> 8);
    lcd_wr_data(0x00FF & y_star);
    lcd_wr_data(y_end >> 8);
    lcd_wr_data(0x00FF & y_end);

    lcd_write_ram_prepare();
}

void lcd_direction(uint8_t direction)
{
    g_lcd_dev.setxcmd = 0x2A;
    g_lcd_dev.setycmd = 0x2B;
    g_lcd_dev.wramcmd = 0x2C;
    g_lcd_dev.rramcmd = 0x2E;
    g_lcd_dev.dir = direction % 4;
    switch (g_lcd_dev.dir) {
        case 0:
            g_lcd_dev.width = LCD_W;
            g_lcd_dev.height = LCD_H;
            lcd_wr_reg(0x36, (1 << 3) | (1 << 6));
            break;
        case 1:
            g_lcd_dev.width = LCD_H;
            g_lcd_dev.height = LCD_W;
            lcd_wr_reg(0x36, (1 << 3) | (1 << 5));
            break;
        case 2:
            g_lcd_dev.width = LCD_W;
            g_lcd_dev.height = LCD_H;
            lcd_wr_reg(0x36, (1 << 3) | (1 << 7));
            break;
        case 3:
            g_lcd_dev.width = LCD_H;
            g_lcd_dev.height = LCD_W;
            lcd_wr_reg(0x36, (1 << 3) | (1 << 7) | (1 << 6) | (1 << 5));
            break;
        default:
            break;
    }
}
