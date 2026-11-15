# Watchdog

> Watchdog 驱动 | sample: `src/application/samples/peripheral/watchdog/watchdog_demo.c`

## 学习目标

- 理解看门狗的原理——硬件倒计时器，超时未喂狗则触发系统复位
- 掌握 `uapi_watchdog_init` / `uapi_watchdog_enable` / `uapi_watchdog_kick` 的标准用法
- 能够区分"超时复位"和"正常喂狗"两种场景，并在实际工程中选择合适的超时值和喂狗策略

## 基本概念

### 看门狗做什么

看门狗是一个独立于 CPU 的硬件倒计时器——一旦启动就从初始值往下数。如果程序正常，会周期性"喂狗"（把计数器重置回初始值）；如果程序跑飞（死循环、内存踩踏、任务卡死），无法喂狗，计数器减到 0 时硬件直接复位整个系统：

```mermaid
flowchart TD
    S[系统正常运行]
    S -->|任务定期喂狗| S
    S -->|某任务卡死<br/>未喂狗| T[看门狗计数器减到0]
    T --> R[硬件复位]
    R --> B[系统重新启动]
    B --> S
```

### 超时值的选择策略

超时值必须大于系统内**最长不可中断操作**的时间：

| 阻塞操作 | 典型耗时 | 超时建议 |
|----------|:---:|:---:|
| Flash 扇区擦除 | ~50ms | 不影响（远小于超时） |
| Flash 全片擦除 | 数秒 | 擦除期间需临时喂狗或延长超时 |
| OTA (Over-The-Air) 固件下载 | 数秒~数十秒 | 下载循环中必须喂狗 |
| 正常任务循环 | < 100ms | 喂狗间隔 = 超时 / 2 最安全 |

> sample 默认超时 2 秒（`TIME_OUT=2`），喂狗间隔 500ms——喂狗间隔远小于超时的一半，留出充分的安全余量。

### sample 的两种配置

本 sample 通过 Kconfig 控制两种行为：
- **`CONFIG_WDT_TIMEOUT_SAMPLE`**：启动看门狗后进入 `while(1){}` 死循环——不喂狗，2 秒后系统复位。用于验证看门狗确实能复位系统
- **`CONFIG_WDT_KICK_SAMPLE`**：启动看门狗后在循环中每 500ms 喂狗——系统稳定运行不复位。用于验证喂狗机制正常工作

## 涉及 API

| API | 用途 | 头文件 |
|-----|------|--------|
| `uapi_watchdog_init(timeout)` | 初始化看门狗，设置超时时间（秒） | `watchdog.h` |
| `uapi_watchdog_enable(mode)` | 使能看门狗并设置工作模式 | `watchdog.h` |
| `uapi_register_watchdog_callback(cb)` | 注册超时前回调（预警通知） | `watchdog.h` |
| `uapi_watchdog_kick()` | 喂狗——重置计数器到初始值 | `watchdog.h` |
| `uapi_watchdog_deinit()` | 反初始化看门狗 | `watchdog.h` |

## 案例说明

### 案例简介

演示看门狗的两种典型场景：
1. **复位验证**（`CONFIG_WDT_TIMEOUT_SAMPLE`）：启动看门狗后不喂狗，进入死循环，2 秒后系统自动复位——验证看门狗硬件功能
2. **正常喂狗**（`CONFIG_WDT_KICK_SAMPLE`）：启动看门狗后每 500ms 喂狗一次，系统持续稳定运行——验证喂狗 API 正确性

### 功能规格

| 规格项 | 说明 |
|--------|------|
| 超时时间 | 2 秒（`TIME_OUT=2`） |
| 工作模式 | 1（`WDT_MODE=1`，复位模式） |
| 超时回调 | `watchdog_callback`——超时前最后一次通知 |
| 喂狗间隔 | 500ms（远小于 2s 超时） |
| 喂狗 API | `uapi_watchdog_kick()` |

### 案例流程

```mermaid
sequenceDiagram
    participant T as watchdog_task
    participant W as 看门狗硬件

    T->>W: uapi_watchdog_init(2)
    W->>W: 设置超时 = 2s
    T->>W: uapi_watchdog_enable(RESET_MODE)
    W->>W: 启动倒计时

    alt CONFIG_WDT_TIMEOUT_SAMPLE
        Note over T: while(1){} —— 不喂狗
        W->>W: 2s 后计数器归零
        W->>W: 硬件复位系统
    else CONFIG_WDT_KICK_SAMPLE
        loop 每 500ms
            T->>T: osal_msleep(500)
            T->>W: uapi_watchdog_kick
            W->>W: 计数器重置为 2s
            T->>T: print "kick success"
        end
    end
```

## 案例操作指导

1. 编译：
   ```bash
   fbb build watchdog
   ```
2. 测试复位模式——在 `menuconfig` 中选中 `CONFIG_WDT_TIMEOUT_SAMPLE`：
   - 烧录后串口打印 `init watchdog`
   - 2 秒后系统复位，串口重新输出启动日志
   - 验证看门狗复位功能正常
3. 测试喂狗模式——在 `menuconfig` 中选中 `CONFIG_WDT_KICK_SAMPLE`：
   - 烧录后串口每 500ms 打印 `kick success`
   - 系统持续运行不复位
   - 验证喂狗机制正常

## 关键配置

| 配置项 | 推荐值 | 说明 |
|--------|:---:|------|
| `TIME_OUT` | 2~5 秒 | 小于 2s 有误复位风险；大于 5s 则系统死机后需等太久才复位。sample 用 2s 便于快速观察 |
| `WDT_MODE` | 1（复位模式） | `0`=仅中断不复位（用于调试），`1`=超时复位（量产默认）。调试阶段可设 0 避免频繁复位干扰定位 |
| 喂狗位置 | 最高优先级监控任务 | 决不能在各功能任务中各喂各的——若监控任务卡死而某个低优先级任务仍在喂狗，看门狗失效 |
| 超时回调 | 仅作日志/告警 | `watchdog_callback` 在超时前极短时间窗口触发——只能做最少操作（记录日志、设置标志），不能做耗时操作 |

> **Trade-off**：超时值过小 → 正常操作可能误触发复位（如 Flash 擦除时的短暂阻塞）；超时值过大 → 系统死机后恢复时间过长。2~5 秒是嵌入式系统的经验平衡点。

## 代码详解

### 1. 超时回调

超时回调在看门狗计数器即将归零时触发——这是复位前最后一次"呼救"机会，只应做最简单的日志记录：

```c
static errcode_t watchdog_callback(uintptr_t param)
{
    UNUSED(param);
    osal_printk("watchdog kick timeout!\r\n");
    return ERRCODE_SUCC;
}
```

> 实际量产代码中，超时回调应记录复位原因到非易失存储（Flash/EEPROM），以便下次启动时判断上次是否为看门狗复位。

### 2. 初始化看门狗

`uapi_watchdog_init` 的参数 `TIME_OUT` 单位为**秒**（非毫秒）。返回值检查 `ERRCODE_INVALID_PARAM` 用于防御无效超时值：

```c
errcode_t ret = uapi_watchdog_init(TIME_OUT);  /* TIME_OUT = 2 秒 */
if (ret == ERRCODE_INVALID_PARAM) {
    osal_printk("param is error, timeout is %d.\r\n", TIME_OUT);
    return NULL;
}
(void)uapi_watchdog_enable((wdt_mode_t)WDT_MODE);
(void)uapi_register_watchdog_callback(watchdog_callback);
osal_printk("init watchdog\r\n");
```

### 3. 超时复位场景（不喂狗）

使能 `CONFIG_WDT_TIMEOUT_SAMPLE` 后，任务进入死循环，看门狗 2 秒后自动复位：

```c
#if defined(CONFIG_WDT_TIMEOUT_SAMPLE)
    while (1) {};   /* 故意不喂狗 —— 2s 后系统复位 */
#endif
```

### 4. 正常喂狗场景

使能 `CONFIG_WDT_KICK_SAMPLE` 后，任务每 500ms 调用 `uapi_watchdog_kick()` 重置计数器：

```c
#if defined(CONFIG_WDT_KICK_SAMPLE)
    while (1) {
        osal_msleep(WDT_TASK_DURATION_MS);   /* 500ms */
        (void)uapi_watchdog_kick();          /* 喂狗 —— 重置 2s 倒计时 */
        osal_printk("kick success\r\n");
    }
#endif
```

### 5. 反初始化（正常退出路径）

当不复位也不喂狗时（两个 Kconfig 都未开启），执行 `deinit` 退出：

```c
(void)uapi_watchdog_deinit();
return NULL;
```

> 喂狗必须放在一个**独立且不会卡死**的任务中。如果系统有多个任务，建议创建一个最高优先级的"系统监控任务"专门负责喂狗——而不是在业务任务中顺手喂狗。

---

