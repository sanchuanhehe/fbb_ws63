# Timer

> Timer 驱动 | sample: `src/application/samples/peripheral/timer/timer_demo.c` | 前置：[周期定时器](../../os/timer/periodic-timer.md)

## 学习目标

- 理解硬件定时器与 OS (Operating System) 软件定时器的区别——硬件定时器精度可达微秒级、回调运行在硬件 ISR (Interrupt Service Routine) 上下文中
- 掌握 `uapi_timer_init` / `uapi_timer_create` / `uapi_timer_start` / `uapi_timer_stop` / `uapi_timer_delete` 的完整生命周期管理
- 能够使用 `uapi_tcxo_get_ms()` 与硬件定时器配合，精确测量定时精度

## 基本概念

### 硬件定时器 vs OS 软件定时器

| 对比项 | 硬件定时器 | OS 软件定时器 |
|--------|:---:|:---:|
| 时钟源 | 芯片硬件定时器（APB 时钟） | OS tick（Systick 1ms） |
| 精度 | μs 级 | ms 级（受 tick 分辨率限制） |
| 回调上下文 | **硬件 ISR** | 定时器软中断 / 任务上下文 |
| 可调阻塞 API | 绝对不能 | 绝对不能 |
| 可用数量 | 受硬件通道数限制（WS63 有多个） | 软件支持（几十个） |
| 典型用途 | 精密时序、PWM (Pulse Width Modulation) 生成、波形测量 | 通用周期性任务、超时处理 |

> **关键约束**：硬件定时器回调在 ISR 中执行——不能调用 `osal_msleep`、`osal_printk`、`printf` 等阻塞或耗时 API。回调中只能做简单操作（翻转 GPIO (General Purpose Input/Output)、递增计数器、`sem_up` 信号量通知任务）。

### sample 架构：多定时器同步测量

本 sample 创建 4 个硬件定时器，分别配置 1ms / 2ms / 3ms / 4ms 延迟，记录每个定时器的启动时间和回调触发时间，最后打印实际延迟与理论值的对比：

```mermaid
flowchart TD
    INIT[uapi_timer_init] --> ADAPT[uapi_timer_adapter 绑定 IRQ]
    ADAPT --> LOOP[for i=0..3]
    LOOP --> CREATE[uapi_timer_create]
    CREATE --> RECORD[记录 start_time]
    RECORD --> START[uapi_timer_start delay=1+i ms]
    START --> DELAY[osal_msleep 5ms 错开启动]
    DELAY --> LOOP
    LOOP --> WAIT[等待 4 个回调全部触发]
    WAIT --> STOP[逐个 stop + delete]
    STOP --> PRINT[打印 real_time vs delay_time]
```

## 涉及 API

| API | 用途 | 头文件 |
|-----|------|--------|
| `uapi_timer_init()` | 初始化定时器模块 | `timer.h` |
| `uapi_timer_adapter(index, irqn, prio)` | 绑定定时器硬件通道与中断号 | `timer.h` |
| `uapi_timer_create(index, &handle)` | 创建定时器实例（获取句柄） | `timer.h` |
| `uapi_timer_start(handle, delay_us, cb, data)` | 启动单次/周期定时器（微秒） | `timer.h` |
| `uapi_timer_stop(handle)` | 停止定时器 | `timer.h` |
| `uapi_timer_delete(handle)` | 删除定时器实例 | `timer.h` |
| `uapi_tcxo_get_ms()` | 获取系统毫秒时间戳（TCXO (Temperature Compensated Crystal Oscillator) 时钟） | `tcxo.h` |

## 案例说明

### 案例简介

创建 4 个硬件定时器，配置不同延迟（1000/2000/3000/4000 微秒），在每个定时器的回调中记录实际触发时间戳，待所有回调完成后打印每个定时器的实际延迟与理论延迟的对比。

### 功能规格

| 规格项 | 说明 |
|--------|------|
| 定时器数量 | 4 个 |
| 延迟配置 | 1000us / 2000us / 3000us / 4000us |
| 定时器通道 | 硬件通道 1（TIMER_INDEX=1） |
| 测量手段 | `uapi_tcxo_get_ms()` 记录启动和回调时间 |
| 回调功能 | 记录 `end_time` + 递增 `g_timer_int_count` |

### 案例流程

```mermaid
sequenceDiagram
    participant T as timer_task
    participant H as 硬件定时器1
    participant C as 回调 ISR

    T->>H: uapi_timer_init
    T->>H: uapi_timer_adapter(1, IRQN, PRIO)
    T->>H: uapi_timer_create → handle0
    T->>T: start_time0 = tcxo_get_ms
    T->>H: uapi_timer_start(handle0, 1000us, cb, 0)
    T->>T: osal_msleep(5) 错开启动

    Note over H: 1ms 后
    H->>C: 回调: end_time0 = tcxo_get_ms
    C->>C: g_timer_int_count++

    Note over T: ... 同理创建 handle1~3 ...
    Note over T: 等待 g_timer_int_count == 4
    T->>H: uapi_timer_stop + delete ×4
    T->>T: 打印 real_time[i] vs delay_time[i]
```

## 案例操作指导

1. 编译：
   ```bash
   fbb build timer
   ```
2. 烧录固件，串口观察输出：
   ```
   real time[0] = xxxms    delay = 1ms
   real time[1] = xxxms    delay = 2ms
   real time[2] = xxxms    delay = 3ms
   real time[3] = xxxms    delay = 4ms
   ```
3. `real time` 应接近理论延迟值（允许 ±1ms 的 TCXO 时钟测量误差）

## 关键配置

| 配置项 | 推荐值 | 说明 |
|--------|:---:|------|
| `delay_time` 单位 | 微秒 | `uapi_timer_start` 的 `delay` 参数单位为 **微秒**，`1ms = 1000` |
| `TIMER_INDEX` | 1（或 SOC 定义） | 不同通道对应不同的硬件定时器 IP (Internet Protocol)。WS63 的通道号和 IRQ (Interrupt Request) 号需查芯片手册 |
| `timer_adapter` 优先级 | 1（低优先级） | ISR 优先级不能高于系统关键 ISR（如 Systick）。通常设 1~3 |
| 启动错开 | `osal_msleep(5)` | 连续 `start` 多个定时器时，用短延时错开启动时间，避免 TCXO 时间戳重叠 |

> **Trade-off**：硬件定时器精度高但通道有限（通常 2~6 个），且回调约束严格。对于不需要 μs 级精度的周期性任务，优先使用 OS 软件定时器——资源更充裕且回调约束更宽松。

## 代码详解

### 1. 数据结构定义

每个定时器关联一个 `timer_info_t` 记录起始时间和延迟值：

```c
typedef struct timer_info {
    uint32_t start_time;   /* 启动时的 TCXO 毫秒时间戳 */
    uint32_t end_time;     /* 回调触发时的 TCXO 毫秒时间戳 */
    uint32_t delay_time;   /* 配置的延迟（微秒） */
} timer_info_t;

static uint32_t g_timer_int_count = 0;
static timer_info_t g_timers_info[TIMER_TIMERS_NUM] = {
    {0, 0, TIMER1_DELAY_1000US},   /* 1000us */
    {0, 0, TIMER2_DELAY_2000US},   /* 2000us */
    {0, 0, TIMER3_DELAY_3000US},   /* 3000us */
    {0, 0, TIMER4_DELAY_4000US}    /* 4000us */
};
```

### 2. 定时器回调（ISR 上下文）

回调通过 `data` 参数传递定时器索引，在 ISR 中仅做时间记录和计数递增——不能做任何阻塞操作：

```c
static void timer_timeout_callback(uintptr_t data)
{
    uint32_t timer_index = (uint32_t)data;
    g_timers_info[timer_index].end_time = uapi_tcxo_get_ms();  /* 记录触发时间 */
    g_timer_int_count++;  /* 递增全局计数 */
}
```

### 3. 定时器初始化与创建

先初始化模块，绑定硬件通道与中断号，然后循环创建 4 个定时器实例并依次启动：

```c
timer_handle_t timer_index[TIMER_TIMERS_NUM] = { 0 };
uapi_timer_init();
uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);

for (uint32_t i = 0; i < TIMER_TIMERS_NUM; i++) {
    uapi_timer_create(TIMER_INDEX, &timer_index[i]);         /* 创建实例 */
    g_timers_info[i].start_time = uapi_tcxo_get_ms();        /* 记录启动时间 */
    uapi_timer_start(timer_index[i], g_timers_info[i].delay_time,
                     timer_timeout_callback, i);             /* 启动（us） */
    osal_msleep(TIMER_DELAY_INT);  /* 5ms 间隔，错开各定时器启动 */
}
```

### 4. 等待完成并清理

轮询 `g_timer_int_count` 等待所有回调触发完毕，然后逐个停止并删除定时器，最后打印实际延迟：

```c
while (g_timer_int_count < TIMER_TIMERS_NUM) {
    osal_msleep(TIMER_DELAY_INT);
}

for (uint32_t i = 0; i < TIMER_TIMERS_NUM; i++) {
    uapi_timer_stop(timer_index[i]);
    uapi_timer_delete(timer_index[i]);
    osal_printk("real time[%d] = %dms  ", i,
                (g_timers_info[i].end_time - g_timers_info[i].start_time));
    osal_printk("  delay = %dms\r\n",
                g_timers_info[i].delay_time / TIMER_MS_2_US);
}
```

> 硬件定时器使用完毕后必须调用 `uapi_timer_delete` 释放资源——定时器句柄是有限资源，泄漏会导致后续定时器创建失败。

---

