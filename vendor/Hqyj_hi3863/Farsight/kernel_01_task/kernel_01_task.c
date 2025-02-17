#include "soc_osal.h"
#include "app_init.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include <stdio.h>
osThreadId_t Task1_ID;   //  任务1 ID
osThreadId_t Task2_ID;   //  任务2 ID

/**
 * @description: 任务1为低优先级任务
 * @param {*}
 * @return {*}
 */
void Task1(void *argument) 
{
    unused(argument);
    while (1)
    {
        printf("Task 1.......\n");
        osDelay(100);  
    }
}
/**
 * @description: 任务2为高优先级任务
 * @param {*}
 * @return {*}
 */
void Task2(void *argument)
{
    unused(argument);
    while (1) 
    {
        printf("Task 2: 挂起task1\n");
        osThreadSuspend(Task1_ID);      // 挂起任务1
        osDelay(500);            // 延时5秒钟

        printf("Task 2: 恢复task1\n");
        osThreadResume(Task1_ID);      // 恢复任务1
        osDelay(500);            // 延时5秒钟
    }
}
static void kernel_task_example(void)
{
    osThreadAttr_t attr;
    attr.name       = "Task1";   // 任务的名字
    attr.attr_bits  = 0U;        // 属性位
    attr.cb_mem     = NULL;      // 堆空间地址
    attr.cb_size    = 0U;        // 堆空间大小
    attr.stack_mem  = NULL;      // 栈空间地址
    attr.stack_size = 0x1000;    // 栈空间大小 单位:字节
    attr.priority   = osPriorityNormal; // 任务的优先级，数据越大，优先级越高
    
    Task1_ID=osThreadNew((osThreadFunc_t)Task1, NULL, &attr); // 创建任务1
    if(Task1_ID != NULL) {
           printf("ID = %d, Create Task1_ID is OK!\n", Task1_ID);      
    }
    attr.name = "Task2"; 
    attr.priority = osPriorityNormal1;
    Task2_ID=osThreadNew((osThreadFunc_t)Task2, NULL, &attr);  // 创建任务2
    if(Task2_ID != NULL) {
    
           printf("ID = %d, Create Task2_ID is OK!\n", Task2_ID);
    }
}

/* Run the sample. */
app_run(kernel_task_example);