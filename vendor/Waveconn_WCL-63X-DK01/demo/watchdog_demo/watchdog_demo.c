#include "pinctrl.h"
#include "soc_osal.h"
#include "app_init.h"
#include "watchdog.h"  //调用看门狗相关函数
#define TIME_OUT                  2                   // 看门狗超时时间，单位为秒
#define WDT_MODE                  1                   // 看门狗模式
#define TEST_PARAM_KICK_TIME      10                  // 测试参数，喂狗时间间隔
#define WDT_TASK_DURATION_MS      2000                 // 看门狗任务的执行周期，单位为毫秒

#define WDT_TASK_PRIO             24                  // 看门狗任务的优先级
#define WDT_TASK_STACK_SIZE       0x1000              // 看门狗任务的堆栈大小

// 看门狗超时回调函数
static errcode_t watchdog_callback(uintptr_t param)
{
    UNUSED(param);                                     // 忽略回调函数的参数
    osal_printk("watchdog kick timeout!\r\n");         // 打印超时信息
    return ERRCODE_SUCC;                               // 返回成功状态码
}

// 看门狗任务函数
static void *watchdog_task(const char *arg)
{
    UNUSED(arg);                                       // 忽略任务函数的参数
    // 初始化看门狗，设置超时时间为 TIME_OUT 秒
    errcode_t ret = uapi_watchdog_init(TIME_OUT);
    if (ret == ERRCODE_INVALID_PARAM) {                // 检查初始化是否成功
        osal_printk("param is error, timeout is %d.\r\n", TIME_OUT);
        return NULL; 
    }
    // 启用看门狗，设置工作模式为 WDT_MODE
    (void)uapi_watchdog_enable((wdt_mode_t)WDT_MODE);
    // 注册看门狗超时回调函数
    (void)uapi_register_watchdog_callback(watchdog_callback);
    osal_printk("init watchdog\r\n");                 

#if defined(CONFIG_WDT_TIMEOUT_SAMPLE)                 // 如果定义了超时测试模式
    while (1) {                                        // 无限循环，等待超时
        // 此处不执行任何操作，等待看门狗超时
    }
#endif

#if defined(CONFIG_WDT_KICK_SAMPLE)                   // 如果定义了喂狗测试模式
    while (1) {                                        // 无限循环，持续喂狗
        osal_msleep(WDT_TASK_DURATION_MS);             // 任务休眠 WDT_TASK_DURATION_MS 毫秒
        (void)uapi_watchdog_kick();                    // 执行喂狗操作
        osal_printk("kick success\r\n");               // 打印喂狗成功信息
    }
#endif

    // 去初始化看门狗（正常情况下不会执行到这里）
    (void)uapi_watchdog_deinit();
    return NULL;
}

// 看门狗任务入口函数
static void watchdog_entry(void)
{
    osal_task *task_handle = NULL;                     // 定义任务句柄
    osal_kthread_lock();                               // 锁定内核线程操作
    // 创建看门狗任务
    task_handle = osal_kthread_create((osal_kthread_handler)watchdog_task, 0, "WatchdogTask", WDT_TASK_STACK_SIZE);
    if (task_handle != NULL) {                         // 检查任务是否创建成功
        osal_kthread_set_priority(task_handle, WDT_TASK_PRIO); // 设置任务优先级
        osal_kfree(task_handle);                       // 释放任务句柄资源
    }
    osal_kthread_unlock();                             // 解锁内核线程操作
}

/* 运行看门狗任务入口函数 */
app_run(watchdog_entry);