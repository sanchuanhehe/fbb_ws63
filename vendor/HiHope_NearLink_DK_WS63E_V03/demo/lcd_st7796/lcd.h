#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "spi.h"
#include "lvgl.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    uint16_t id;
    uint8_t  dir;
    uint16_t wramcmd;
    uint16_t rramcmd;
    uint16_t setxcmd;
    uint16_t setycmd;
} lcd_dev_t;

#define USE_HORIZONTAL      0 
#define USE_VERTICAL        1

#define LCD_W               240
#define LCD_H               320
#define DMA_SIZE            4000
#define LCD_BUF_CNT         10
#define LCD_BUF_MAX         (LCD_W * LCD_H * 2 / LCD_BUF_CNT)

#define WHITE               0xFFFF
#define BLACK               0x0000
#define BLUE                0x001F
#define BRED                0XF81F
#define GRED                0XFFE0
#define GBLUE               0X07FF
#define RED                 0xF800
#define MAGENTA             0xF81F
#define GREEN               0x07E0
#define CYAN                0x7FFF
#define YELLOW              0xFFE0
#define BROWN               0XBC40
#define BRRED               0XFC07
#define GRAY                0X8430
#define DARKBLUE            0X01CF
#define LIGHTBLUE           0X7D7C
#define GRAYBLUE            0X5458
#define LIGHTGREEN          0X841F
#define LIGHTGRAY           0XEF5B
#define LGRAY               0XC618
#define LGRAYBLUE           0XA651
#define LBBLUE              0X2B12

void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_set_windows(uint16_t x_star, uint16_t y_star, uint16_t x_end, uint16_t y_end);
void lcd_wr_reg(uint8_t lcd_reg, uint16_t lcd_reg_value);
void lcd_wr_data(uint8_t data);
void lcd_diplay(uint8_t *data, uint32_t pixel_len);
void lcd_write_ram_prepare(void);
void lcd_direction(uint8_t direction);

extern lv_display_t *lv_display;
extern uint8_t g_lcd_buf[LCD_BUF_MAX];
extern uint8_t g_lcd_buf2[LCD_BUF_MAX];

#endif
