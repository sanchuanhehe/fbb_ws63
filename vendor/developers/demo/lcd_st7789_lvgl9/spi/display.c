/**
 * Copyright (c) EndyTsang 2025. All rights reserved. \n
 *
 * Description: LVGL9 DEMO. \n
 *
 */

#include "display.h"

static timer_handle_t timer_handle = NULL;                     // 定时器句柄
static uint8_t buf_1_1[MY_DISP_HOR_RES * 10 * BYTE_PER_PIXEL]; /*A buffer for 10 rows*/
void TimerCallback(uintptr_t data)
{
    unused(data);
    lv_tick_inc(1); // 告诉LVGL时间过去1毫秒
    uapi_timer_start(timer_handle, TICK_US, TimerCallback, 0);
}

static void disp_flush(lv_display_t *disp_drv, const lv_area_t *area, uint8_t *px_map)
{
    st7789_set_window(area->x1, area->y1, area->x2, area->y2);
    st7789_write_data(px_map, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2); // 2字节每个像素
    lv_display_flush_ready(disp_drv);
}

static void *spi_master_task(const char *arg)
{
    unused(arg);

    st7789_init();

    lv_init();

    lv_display_t *disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_display_set_flush_cb(disp, disp_flush);
    lv_display_set_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    uapi_timer_init();                                             // 初始化定时器模块
    uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIORITY); // 配置定时器适配器
    uapi_timer_create(TIMER_INDEX, &timer_handle);                 // 创建定时器，并获取定时器句柄
    uapi_timer_start(timer_handle, TICK_US, TimerCallback, 0);     // 启动定时器，设置触发时间和回调函数

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello WS63");
    lv_obj_center(label);

    while (1) {
        lv_timer_handler();
        osal_msleep(SPI_TASK_DURATION_MS);
    }

    return NULL;
}

static void spi_master_entry(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)spi_master_task, 0, "SpiMasterTask", SPI_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SPI_TASK_PRIORITY);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the spi_master_entry. */
app_run(spi_master_entry);