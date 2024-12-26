/*
 * Copyright (c) 2024 HiSilicon Technologies CO., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SSD1306_H
#define SSD1306_H

#include <stddef.h>
#include <stdint.h>
#include "ssd1306_fonts.h"

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT 64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH 128
#endif

// some LEDs don't display anything in first two columns

#ifndef SSD1306_BUFFER_SIZE
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * SSD1306_HEIGHT / 8)
#endif

// Enumeration for screen colors
typedef enum {
    BLACK = 0x00, // BLACK color, no pixel
    WHITE = 0x01  // Pixel is set. Color depends on OLED
} ss_d1306_color;

typedef enum {
    SSD1306_OK = 0x00,
    SSD1306_ERR = 0x01 // Generic error.
} ss_d1306_error_t;

// Struct to store transformations
typedef struct {
    uint16_t current_x;
    uint16_t current_y;
    uint8_t inverted;
    uint8_t initialized;
    uint8_t display_on;
} SSD1306_t;
typedef struct {
    uint8_t x;
    uint8_t y;
} ss_d1306_vertex;

// Procedure definitions
void ssd1306_init(void);
void ssd1306_fill(ss_d1306_color color);
void ssd1306_set_cursor(uint8_t x, uint8_t y);
void ssd1306_update_screen(void);

char ssd1306_draw_char(char ch, font_def font, ss_d1306_color color);
char ssd1306_draw_string(char *str, font_def font, ss_d1306_color color);

void ssd1306_draw_pixel(uint8_t x, uint8_t y, ss_d1306_color color);
void ssd1306_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ss_d1306_color color);
void ssd1306_draw_polyline(const ss_d1306_vertex *par_vertex, uint16_t par_size, ss_d1306_color color);
void ssd1306_draw_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, ss_d1306_color color);
void ssd1306_draw_circle(uint8_t par_x, uint8_t par_y, uint8_t par_r, ss_d1306_color par_color);
void ssd1306_draw_bitmap(const uint8_t *bitmap, uint32_t size);
void ssd1306_draw_region(uint8_t x, uint8_t y, uint8_t w, const uint8_t *data, uint32_t size);

/**
 * @brief Sets the contrast of the display.
 * @param[in] value contrast to set.
 * @note Contrast increases as the value increases.
 * @note RESET = 7Fh.
 */
void ssd1306_set_contrast(const uint8_t value);
/**
 * @brief Set Display ON/OFF.
 * @param[in] on 0 for OFF, any for ON.
 */
void ssd1306_setdisplay_on(const uint8_t on);
/**
 * @brief Reads display_on state.
 * @return  0: OFF.
 *          1: ON.
 */
uint8_t ssd1306_getdisplay_on(void);

// Low-level procedures
void ssd1306_reset(void);
void ssd1306_write_command(uint8_t byte);
void ssd1306_write_data(uint8_t *buffer, size_t buff_size);
ss_d1306_error_t ssd1306_fill_buffer(uint8_t *buf, uint32_t len);
void ssd1306_clear_oled(void);
void ssd1306_printf(char *fmt, ...);

#endif // __SSD1306_H__