#include "pinctrl.h"
#include "watchdog.h"
#include "soc_osal.h"
#include "app_init.h"
#include "gpio.h"
#include "systick.h"
#include "timer.h"
#include "tcxo.h"
#include "chip_core_irq.h"

#include "lcd.h"
#include "pic_sample.h"

#define FRAME_RATE                  30
#define FRAME_TIME_MS               (uint32_t)(1000/FRAME_RATE)

#define TASK_STACK_SIZE             0x1000
#define TASK_PRIO                   24
#define TIMER_TASK_PRIO             25

#define TIMER_INDEX                 1
#define TIMER_PRIO                  1
#define TIMER_DELAY_INT             5
#define TIMER1_DELAY_FPS            (FRAME_TIME_MS*1000)
#define TIMER_MS_2_US               1000

typedef struct timer_info {
    uint32_t start_time;
    uint32_t end_time;
    uint32_t delay_time;
} timer_info_t;

static timer_info_t g_timers_info = {0, 0, 1000};
static timer_handle_t timer1 = { 0 };
lv_display_t *lv_display;

/* Timed task callback function list. */
static void timer_timeout_callback(uintptr_t data)
{
    lv_tick_inc(1); // feed 1ms tic to lvgl
    uapi_timer_start(timer1, g_timers_info.delay_time, timer_timeout_callback, (uint32_t)data);
}

static int *timer_task(const char *arg)
{
    unused(arg);
    uapi_timer_init();
    uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);

    uapi_timer_create(TIMER_INDEX, &timer1);
    g_timers_info.start_time = uapi_tcxo_get_ms();
    uapi_timer_start(timer1, g_timers_info.delay_time, timer_timeout_callback, 0);

    while (1) {
        osal_msleep(TIMER_DELAY_INT);
    }

    return 0;
}

static void disp_flush(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map)
{
    lcd_diplay(px_map, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2); // RGB565 is 16 bits in a pixel.
    lv_display_flush_ready(disp_drv);
}

static int spi_st7796_task(const char *arg)
{
    unused(arg);
    uapi_systick_init();
    uapi_tcxo_init();
    lcd_init();

    lv_init();
    lv_display = lv_display_create(LCD_W, LCD_H);
    lv_display_set_flush_cb(lv_display, disp_flush);
    lv_display_set_buffers(lv_display, g_lcd_buf, g_lcd_buf2, sizeof(g_lcd_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_obj_t *img = lv_image_create(lv_screen_active());
    lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);
    
    // 创建图片描述符
    static lv_image_dsc_t img_dsc[] = {
        {
            .header.cf = LV_COLOR_FORMAT_RGB565,
            .header.w = LCD_W,
            .header.h = LCD_H,
            .data_size = LCD_W * LCD_H * 2,
            .data = pic1,
        },
        {
            .header.cf = LV_COLOR_FORMAT_RGB565,
            .header.w = LCD_W,
            .header.h = LCD_H,
            .data_size = LCD_W * LCD_H * 2,
            .data = pic2,
        },
        {
            .header.cf = LV_COLOR_FORMAT_RGB565,
            .header.w = LCD_W,
            .header.h = LCD_H,
            .data_size = LCD_W * LCD_H * 2,
            .data = pic3,
        }
    };

    uint32_t i = 0;
    while (1) {
        lv_timer_handler();
        lv_image_set_src(img, &img_dsc[i%3]);
        lv_obj_center(img);
        i++;
    }

    return 0;
}

static void st7796_task_entry(void)
{
    osal_task *task_handle = NULL;

    osal_printk("================== LCD st7796 + lvgl 9.4 sample start. =================\n");
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)spi_st7796_task, 0, "LcdST7796Task", TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TASK_PRIO);
        osal_kfree(task_handle);
    }

    task_handle = osal_kthread_create((osal_kthread_handler)timer_task, 0, "TimerTask", TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TIMER_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the entry. */
app_run(st7796_task_entry);