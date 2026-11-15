# TSensor 片上温度监测

> 使用技术：片上温度传感器、周期读取、过温阈值中断

本案例周期读取 WS63 芯片结温，并在结温达到配置阈值时通过回调记录过温事件。

## 学习目标

- 理解芯片结温与环境温度的区别
- 使用 TSensor 接口读取当前结温
- 配置过温阈值并处理过温回调

## 基本概念

### 芯片结温

TSensor 是芯片内部的温度传感器，测量的是芯片结温，即芯片内部硅片的温度。结温会受到环境温度、芯片负载、射频工作状态和散热条件影响，不能把它直接当作室内环境温度。

### 周期读取

周期读取适合观察温度随负载变化的趋势。读取间隔太短会产生大量重复日志；间隔太长则可能错过快速变化。本案例默认每 5000 ms 读取一次。

### 过温阈值

过温阈值用于在温度达到指定值时触发硬件事件。回调运行在中断上下文中，应只保存温度和设置标志；打印、降频或关闭业务等处理应放到任务上下文中完成。

## 涉及 API

| API | 用途 |
|-----|------|
| `uapi_tsensor_get_current_temp()` | 读取当前芯片结温 |
| `uapi_tsensor_enable_overtemp_interrupt()` | 设置过温阈值并注册回调 |

TSensor 的初始化和连续采样由 WS63 平台启动流程完成，样例任务等待平台就绪后使用对外接口读取温度并配置过温事件。

## 案例说明

样例按以下流程运行：

```text
等待平台 TSensor 就绪 → 配置过温阈值 → 周期读取结温
                                      └→ 达到阈值后记录并输出过温事件
```

默认过温阈值为 100℃，默认读取周期为 5000 ms。为在常温下验证回调，可以在 Kconfig UI 中把阈值临时设置为低于当前结温的值；测试结束后应恢复适合产品散热设计的阈值。

## 案例操作指导

### 配置样例

打开 `ws63-liteos-app` 的 Kconfig UI，进入：

```text
Application
  → Enable Sample.
    → Enable the Sample of Peripheral.
      → Support TSensor Sample.
```

在 `TSensor Sample Configuration` 中配置：

- `TSensor overtemperature threshold in Celsius.`：过温阈值
- `TSensor read period in milliseconds.`：周期读取间隔

保存配置后构建并烧录：

```powershell
fbb build --clean ws63-liteos-app
fbb flash ws63-liteos-app --port <device_port> --baud 2000000 --json-summary
```

### 周期读取验证

使用默认 100℃ 阈值时，实板输出：

```text
[tsensor] platform sensor ready
[tsensor] overtemp threshold=100C
[tsensor] current temperature=37C
```

温度会随板端负载和环境变化，不能把示例值作为固定期望值。

### 过温回调验证

实测时将阈值临时设置为 30℃，板端结温高于该阈值后输出：

```text
[tsensor] platform sensor ready
[tsensor] overtemp threshold=30C
[tsensor] current temperature=37C
[tsensor] OVERHEAT: temperature=38C
```

出现 `OVERHEAT` 说明过温阈值和回调路径生效。常温测试使用的 30℃ 仅用于快速触发，不能直接作为产品阈值。

## 关键配置

| 配置项 | 默认值 | 范围 | 说明 |
|--------|--------|------|------|
| `CONFIG_SAMPLE_SUPPORT_TSENSOR` | `n` | `y/n` | 启用 TSensor 样例 |
| `CONFIG_TSENSOR_SAMPLE_OVERTEMP_C` | `100` | -40～125℃ | 过温触发阈值 |
| `CONFIG_TSENSOR_SAMPLE_READ_PERIOD_MS` | `5000` | 100～60000 ms | 周期读取间隔 |

## 代码详解

过温回调只更新共享状态，不执行阻塞操作：

```c
static errcode_t tsensor_sample_overtemp_callback(int8_t temp)
{
    g_tsensor_sample_peak_temp = temp;
    g_tsensor_sample_overtemp = true;
    return ERRCODE_SUCC;
}
```

样例任务读取当前结温，并在任务上下文中输出过温事件：

```c
ret = uapi_tsensor_get_current_temp(&current_temp);
if (ret == ERRCODE_SUCC) {
    osal_printk("[tsensor] current temperature=%dC\r\n", current_temp);
}

if (g_tsensor_sample_overtemp) {
    osal_printk("[tsensor] OVERHEAT: temperature=%dC\r\n",
                g_tsensor_sample_peak_temp);
    g_tsensor_sample_overtemp = false;
}
```

