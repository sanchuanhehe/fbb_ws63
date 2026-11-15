# RTC 软件定时器

> 使用技术：RTC 初始化、硬件适配、软件定时器、超时回调

本案例使用 WS63 的 `uapi_rtc_*` 接口创建 RTC 软件定时器。定时器到期后在 RTC 中断上下文执行回调，任务检查回调次数和透传参数，以验证 RTC 定时功能。

## 学习目标

- 使用 `uapi_rtc_init()` 初始化 RTC 驱动
- 使用 `uapi_rtc_adapter()` 适配 RTC 硬件和中断
- 使用 `uapi_rtc_create()` 创建 RTC 软件定时器
- 使用 `uapi_rtc_start()` 启动一次性定时并接收超时回调
- 使用 `uapi_rtc_stop()`、`uapi_rtc_delete()` 和 `uapi_rtc_deinit()` 释放资源

## 涉及 API

| API | 用途 |
|-----|------|
| `uapi_rtc_init()` | 初始化 RTC 驱动 |
| `uapi_rtc_adapter()` | 适配底层 RTC、注册中断 |
| `uapi_rtc_create()` | 创建 RTC 软件定时器并返回句柄 |
| `uapi_rtc_start()` | 设置超时时间、回调和回调参数并启动定时器 |
| `uapi_rtc_stop()` | 停止 RTC 软件定时器 |
| `uapi_rtc_delete()` | 删除 RTC 软件定时器 |
| `uapi_rtc_get_max_ms()` | 获取支持的最大超时时间 |
| `uapi_rtc_int_cnt_record_get()` | 获取 RTC 中断累计次数 |
| `uapi_rtc_deinit()` | 去初始化 RTC 驱动 |

## 案例流程

```text
初始化 RTC
  → 适配 RTC_0 和 RTC_0_IRQN
  → 创建软件定时器
  → 启动一次性定时器
  → RTC 中断回调记录次数及透传参数
  → 重复验证 3 次
  → 停止、删除并去初始化 RTC
```

RTC 回调运行在中断上下文中，因此案例只在回调内更新 `volatile` 变量，不进行阻塞等待或复杂处理。日志输出和验证工作由任务完成。

## 配置案例

打开 `ws63-liteos-app` 的 Kconfig UI，进入：

```text
Application
  → Enable Sample.
    → Enable the Sample of Peripheral.
      → Support RTC Sample.
```

启用 RTC sample 时会自动选择 `DRIVER_SUPPORT_RTC`。在 `RTC Sample Configuration` 中可修改 `RTC sampling period in milliseconds.`，取值范围为 100～60000 ms，默认值为 1000 ms。

当前 `ws63-liteos-app` 配置默认未包含 RTC 组件。编译前需要在 `build/config/target_config/ws63/config.py` 的 `ws63-liteos-app` 配置中，将 `rtc_unified`、`hal_rtc_unified` 和 `rtc_unified_port` 加入 `ram_component`；也可以将组件集合 `rtc_unified` 加入 `ram_component_set`。

配置完成后构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <device_port> --baud 2000000 --json-summary
```

## 预期输出

默认周期为 1000 ms，预期输出类似：

```text
[rtc] ready: period=1000ms max=...ms samples=3
[rtc] sample #1 callback received, irq_count=1
[rtc] sample #2 callback received, irq_count=2
[rtc] sample #3 callback received, irq_count=3
[rtc] verification passed: callbacks=3
```

## 关键配置

| 配置项 | 默认值 | 范围 | 说明 |
|--------|--------|------|------|
| `CONFIG_SAMPLE_SUPPORT_RTC` | `n` | `y/n` | 启用 RTC 软件定时器案例 |
| `CONFIG_DRIVER_SUPPORT_RTC` | 由案例选择 | `y/n` | 启用 RTC 驱动 |
| `CONFIG_RTC_SAMPLE_PERIOD_MS` | `1000` | 100～60000 ms | 每次 RTC 定时的超时时间 |
