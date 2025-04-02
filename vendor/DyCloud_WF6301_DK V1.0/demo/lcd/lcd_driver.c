/*
 * @Description:
 * @Author: Condi
 * @Date: 2025-03-25 16:07:31
 * @FilePath: \src\application\samples\peripheral\lcd\lcd_driver.c
 * @LastEditTime: 2025-03-26 14:47:44
 * @LastEditors: Condi
 */
#include "pinctrl.h"
#include "spi.h"
#include "gpio.h"
#include "soc_osal.h"
#include "app_init.h"
#include "stdint.h"
#include "lcd_driver.h"
#include "lcd_drv9853.h"
#include "img.h"
#include "img_test.h"

#define SPI_SLAVE_NUM 1
#define SPI_FREQUENCY 32
#define SPI_CLK_POLARITY 0
#define SPI_CLK_PHASE 0
#define SPI_FRAME_FORMAT 0
#define SPI_FRAME_FORMAT_STANDARD 0
#define SPI_FRAME_SIZE_8 HAL_SPI_FRAME_SIZE_8
#define SPI_TMOD 0
#define SPI_WAIT_CYCLES 0x10
#if defined(CONFIG_SPI_SUPPORT_DMA) && !(defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH))
#define SPI_DMA_WIDTH 0
#endif

#define LCD_TASK_DURATION_MS 500
#define LCD_TASK_PRIO 24
#define LCD_TASK_STACK_SIZE 0x3000

#define CONFIG_SPI_MASTER_PIN_MODE PIN_MODE_3

#define LCD_CMD_MODE() uapi_gpio_set_val(CONFIG_LCD_RS, GPIO_LEVEL_LOW)
#define LCD_DATA_MODE() uapi_gpio_set_val(CONFIG_LCD_RS, GPIO_LEVEL_HIGH)

#define LCD_WIDTH (240)
#define LCD_HEIGHT (296)

#define COLOR_RED 0x00f8
#define COLOR_GREEN 0xe007
#define COLOR_BLUE 0x1f00
#define COLOR_BLACK 0x0000

static spi_xfer_data_t lcd_spi_data = {
    .tx_buff = NULL,
    .tx_bytes = 0,
    .rx_buff = NULL,
    .rx_bytes = 0,
};

static spi_attr_t lcd_spi_config = {0};
static spi_extra_attr_t lcd_spi_ext_config = {0};

static void lcd_rst_pin_init(void)
{
    uapi_pin_set_mode(CONFIG_LCD_RST, PIN_MODE_0);
    uapi_pin_set_mode(CONFIG_LCD_RS, PIN_MODE_0);
    uapi_gpio_set_dir(CONFIG_LCD_RST, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_dir(CONFIG_LCD_RS, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(CONFIG_LCD_RST, GPIO_LEVEL_LOW);
    osal_msleep(LCD_TASK_DURATION_MS / 10);
    uapi_gpio_set_val(CONFIG_LCD_RST, GPIO_LEVEL_HIGH);
}
static void lcd_spi_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_LCD_DATA, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_LCD_CS, CONFIG_SPI_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_LCD_CLK, CONFIG_SPI_MASTER_PIN_MODE);
}

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
static void lcd_spi_master_write_int_handler(const void *buffer, uint32_t length)
{
    unused(buffer);
    unused(length);
}

static void lcd_spi_master_rx_callback(const void *buffer, uint32_t length, bool error)
{
    if (buffer == NULL || length == 0) {
    }
    if (error) {
        osal_printk("app_spi_master_read_int error!\r\n");
        return;
    }
    uint8_t *buff = (uint8_t *)buffer;
    for (uint32_t i = 0; i < length; i++) {
        osal_printk("buff[%d] = %x\r\n", i, buff[i]);
    }
    osal_printk("app_spi_master_read_int success!\r\n");
}
#endif
static void lcd_spi_master_init_config(void)
{
    lcd_spi_config.is_slave = false;
    lcd_spi_config.slave_num = SPI_SLAVE_NUM;
    lcd_spi_config.bus_clk = SPI_CLK_FREQ;
    lcd_spi_config.freq_mhz = SPI_FREQUENCY;
    lcd_spi_config.clk_polarity = SPI_CLK_POLARITY;
    lcd_spi_config.clk_phase = SPI_CLK_PHASE;
    lcd_spi_config.frame_format = SPI_FRAME_FORMAT;
    lcd_spi_config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    lcd_spi_config.frame_size = SPI_FRAME_SIZE_8;
    lcd_spi_config.tmod = SPI_TMOD;
    lcd_spi_config.sste = 0;

    lcd_spi_ext_config.qspi_param.wait_cycles = 0x10;
#if defined(CONFIG_SPI_MASTER_SUPPORT_QSPI)
    lcd_spi_config.tmod = HAL_SPI_TRANS_MODE_TX;
    lcd_spi_config.sste = 0;
    lcd_spi_config.spi_frame_format = HAL_SPI_FRAME_FORMAT_QUAD;
    lcd_spi_ext_config.qspi_param.trans_type = HAL_SPI_TRANS_TYPE_INST_S_ADDR_Q;
    lcd_spi_ext_config.qspi_param.inst_len = HAL_SPI_INST_LEN_8;
    lcd_spi_ext_config.qspi_param.addr_len = HAL_SPI_ADDR_LEN_24;
    lcd_spi_ext_config.qspi_param.wait_cycles = 0;
#endif

    uapi_spi_init(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_config, &lcd_spi_ext_config);
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    uapi_dma_init();
    uapi_dma_open();
#ifndef CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH
    spi_dma_config_t dma_cfg = {.src_width = 0, .dest_width = 0, .burst_length = 0, .priority = 0};
    if (uapi_spi_set_dma_mode(CONFIG_SPI_MASTER_BUS_ID, true, &dma_cfg) != ERRCODE_SUCC) {
        osal_printk("spi%d master set dma mode fail!\r\n");
    }
#endif
#endif /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (uapi_spi_set_irq_mode(CONFIG_SPI_MASTER_BUS_ID, true, lcd_spi_master_rx_callback,
                              lcd_spi_master_write_int_handler) == ERRCODE_SUCC) {
        osal_printk("spi%d master set irq mode succ!\r\n\n\n", CONFIG_SPI_MASTER_BUS_ID);
    }
#endif
}

static int lcd_drv_find_begin(uint8_t *begin, uint8_t *end, int pos)
{
    uint8_t *p = &begin[pos];
    while ((p + 3) < end) {
        if (((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]) == BEGIN_FLAG) {
            return (&p[4] - begin);
        }
        p++;
    }

    return -1;
}

static int lcd_drv_find_end(uint8_t *begin, uint8_t *end, int pos)
{
    uint8_t *p = &begin[pos];
    while ((p + 3) < end) {
        if ((uint32_t)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]) == END_FLAG) {
            return (&p[0] - begin);
        }
        p++;
    }

    return -1;
}

void lcd_drv_cmd_list(uint8_t *cmd_list, int cmd_cnt)
{
    int i;
    uint8_t *p8;
    for (i = 0; i < cmd_cnt;) {
        int cnt = 0;

        int begin = lcd_drv_find_begin(cmd_list, &cmd_list[cmd_cnt], i);
        if (begin == -1) {
            continue;
            ; // 未找到起始标志，退出循环
        }

        int end = lcd_drv_find_end(cmd_list, &cmd_list[cmd_cnt], begin);
        if (end == -1) {
            continue; // 未找到结束标志，退出循环
        }

        p8 = (uint8_t *)&cmd_list[begin];
        cnt = end - begin - 1;

        if ((uint32_t)((p8[0] << 24) | (p8[1] << 16) | (p8[2] << 8) | p8[3]) == REGFLAG_DELAY_FLAG) {
            osal_msleep(p8[4]);
        } else {
            lcd_spi_data.tx_buff = &p8[0];
            lcd_spi_data.tx_bytes = 1;
            LCD_CMD_MODE();
            uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
            lcd_spi_data.tx_buff = &p8[1];
            lcd_spi_data.tx_bytes = cnt;
            LCD_DATA_MODE();
            uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
        }
        i = end + 4;
    }
}

void lcd_drv_set_zone(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    uint16_t xpo_s1 = (0x00 + x1);
    uint16_t xpo_s2 = (0x00 + x2);
#if SCREEN_VERSION
    uint16_t ypo_s1 = (0x00 + y1);
    uint16_t ypo_s2 = (0x00 + y2);
#else
    uint16_t ypo_s1 = (0x0C + y1);
    uint16_t ypo_s2 = (0x0C + y2);
#endif

    uint8_t lcd_area_cmds[2][4] = {
        {(xpo_s1 >> 8), (xpo_s1 >> 0), (xpo_s2 >> 8), (xpo_s2 >> 0)},
        {(ypo_s1 >> 8), (ypo_s1 >> 0), (ypo_s2 >> 8), (ypo_s2 >> 0)},
    };

    uint8_t cmd = 0x2a;
    lcd_spi_data.tx_buff = &cmd;
    lcd_spi_data.tx_bytes = 1;
    LCD_CMD_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);

    lcd_spi_data.tx_buff = lcd_area_cmds[0];
    lcd_spi_data.tx_bytes = 4;
    LCD_DATA_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);

    cmd = 0x2b;
    lcd_spi_data.tx_buff = &cmd;
    lcd_spi_data.tx_bytes = 1;
    LCD_CMD_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);

    lcd_spi_data.tx_buff = lcd_area_cmds[1];
    lcd_spi_data.tx_bytes = 4;
    LCD_DATA_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
}

void lcd_drv_draw_start(void)
{
    uint8_t cmd = 0x2c;
    lcd_spi_data.tx_buff = &cmd;
    lcd_spi_data.tx_bytes = 1;
    LCD_CMD_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
    LCD_DATA_MODE();
}

void lcd_drv_draw_buf(uint8_t *buff, uint32_t buff_len)
{
    lcd_spi_data.tx_buff = buff;
    lcd_spi_data.tx_bytes = buff_len;
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
}

void lcd_drv_open(void)
{
    uint8_t cmd = 0x11;
    lcd_spi_data.tx_buff = &cmd;
    lcd_spi_data.tx_bytes = 1;
    LCD_CMD_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
    osal_msleep(LCD_TASK_DURATION_MS / 10);
    cmd = 0x29;
    lcd_spi_data.tx_buff = &cmd;
    lcd_spi_data.tx_bytes = 1;
    LCD_CMD_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
    LCD_DATA_MODE();
    osal_msleep(LCD_TASK_DURATION_MS / 20);
}

void lcd_drv_close(void)
{
    uint8_t cmd = 0x28;
    lcd_spi_data.tx_buff = &cmd;
    lcd_spi_data.tx_bytes = 1;
    LCD_CMD_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
    cmd = 0x10;
    lcd_spi_data.tx_buff = &cmd;
    lcd_spi_data.tx_bytes = 1;
    LCD_CMD_MODE();
    uapi_spi_master_write(CONFIG_SPI_MASTER_BUS_ID, &lcd_spi_data, 0xFFFFFFFF);
    LCD_DATA_MODE();
}
/**
 * @brief 分区发送屏幕数据
 * @param img_data 屏幕数据指针
 * @param total_size 屏幕数据总大小（字节）
 */
void lcd_drv_send_partitioned(uint8_t *img_data, uint32_t total_size)
{
    const uint32_t chunk_size = 4000; // 每块大小
    uint32_t remaining = total_size;
    uint32_t offset = 0;
    uint8_t temp_buffer[4096] = {0};

    while (remaining > 0) {
        uint32_t chunk = (remaining > chunk_size) ? chunk_size : remaining;

        memcpy(temp_buffer, img_data + offset, chunk);

        // 发送当前区块
        lcd_drv_draw_buf(temp_buffer, chunk);

        // 更新剩余数据和偏移量
        remaining -= chunk;
        offset += chunk;
    }
}

void lcd_drv_full_color(uint16_t rbg565)
{
    uint16_t line_buff[LCD_WIDTH]; /* 颜色数据 */

    lcd_drv_set_zone(0, LCD_WIDTH - 1, 0, LCD_HEIGHT - 1);
    for (int i = 0; i < LCD_WIDTH; i++) {
        line_buff[i] = rbg565;
    }
    lcd_drv_draw_start();
    for (int i = 0; i < LCD_HEIGHT; i++) {
        lcd_drv_draw_buf((uint8_t *)line_buff, sizeof(line_buff));
    }
}

void lcd_drv_draw_img(uint16_t x, uint16_t y, img_dsc_t *img)
{
    uint16_t img_w = img->header.w;
    uint16_t img_h = img->header.h;

    lcd_drv_set_zone(x, x + img_w - 1, y, y + img_h - 1);

    lcd_drv_draw_start();

    lcd_drv_send_partitioned((uint8_t *)img->data, img->data_size);
    osal_msleep(LCD_TASK_DURATION_MS / 10);
}

void lcd_drv_init(void)
{
    uint8_t temp_buffer[400] = {0};
    lcd_rst_pin_init();
    lcd_spi_init_pin(); /* 初始化SPI PIN */
    lcd_spi_master_init_config();

    memcpy(temp_buffer, LCD_SPI_JD9853_CMDLIST, LCD_SPI_JD9853_CMDLIST_LENGTH);

    lcd_drv_cmd_list((uint8_t *)temp_buffer, LCD_SPI_JD9853_CMDLIST_LENGTH);

    lcd_drv_full_color(COLOR_BLACK);
    lcd_drv_open();
    osal_msleep(LCD_TASK_DURATION_MS);
    lcd_drv_full_color(COLOR_GREEN);
    osal_msleep(LCD_TASK_DURATION_MS);
}

static void *lcd_task(const char *arg)
{
    unused(arg);
    osal_printk("lcd_task start!\r\n");
    lcd_drv_init();
    bool loop = true;

    while (1) {
        osal_msleep(LCD_TASK_DURATION_MS);
        if (loop) {
            lcd_drv_draw_img(0, 0, (img_dsc_t *)&IMG1_RBG565);
        } else {
            lcd_drv_draw_img(0, 0, (img_dsc_t *)&IMG2_RBG565);
        }

        loop = !loop;
    }
    return NULL;
}

static void lcd_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)lcd_task, 0, "LCD_Task", LCD_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, LCD_TASK_STACK_SIZE);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the lcd_entry. */
app_run(lcd_entry);
