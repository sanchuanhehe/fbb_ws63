# 基础雷达

> SLP (SparkLink Positioning) Radar API

## 学习目标

- 理解 SLP 雷达的工作原理——发射探测信号 → 接收反射 → 解析目标信息
- 掌握雷达硬件参数和算法参数的配置
- 掌握结果回调的注册和解析
- 能够在 WS63 上检测人体存在和运动目标

## 规格与功能

配置 SLP 雷达，检测人体存在和运动目标，串口打印目标距离/速度/角度。

| 规格项 | 说明 |
|--------|------|
| 平台 | SLP Radar |
| 检测能力 | 人体存在 / 运动目标追踪（最多 3 个） |
| 检测范围 | 0~700cm（可配置） |
| 结果获取 | 回调模式 / 轮询模式 |

程序运行流程：雷达上电 → 配置硬件参数 → 配置算法参数 → 启动 → 结果回调 → 解析目标信息 → 串口打印。

## 基本概念

### 雷达感知原理

```mermaid
flowchart LR
    TX[发射天线] -->|电磁波| Target[目标]
    Target -->|反射回波| RX[接收天线]
    RX --> DSP[信号处理]
    DSP --> Algo[算法解析]
    Algo --> Result[距离/速度/角度/人体存在]
```

### 硬件参数 vs 算法参数

| 类别 | 配置内容 | 影响 |
|------|---------|------|
| **硬件参数** | 通道/发射功率/天线编码/AGC | 射频性能、检测距离 |
| **算法参数** | 灵敏度/检测范围/退出延迟/AI 补偿 | 检测策略、误报率 |

### 人体存在检测

`radar_result_t` 中的 `is_human_presence` 标志位直接指示房间内是否有人。可配置接近区域阈值（1m/2m/6m），区分离设备不同距离范围的活动。

## 涉及 API

| API | 用途 |
|-----|------|
| `uapi_radar_set_power_status(POWERON)` | 雷达芯片上电 |
| `uapi_radar_set_hardware_para()` | 配置射频和帧参数 |
| `uapi_radar_set_alg_para()` | 配置检测算法参数 |
| `uapi_radar_set_status(START)` | 启动雷达 |
| `uapi_radar_register_result_cb()` | 注册结果回调 |
| `uapi_radar_get_result()` | 轮询获取结果 |

## 案例操作指导


编译烧录后，有人在雷达前移动时串口打印距离和人体存在标志。

## 关键配置

| 参数 | 值 | 说明 |
|------|-----|------|
| 检测范围 | 0~700cm | 默认值 |
| 退出延迟 | 20s | 人离开后延时上报"无人" |
| AI (Artificial Intelligence) 抗干扰 | 开启 | 减少误报 |
| 安装高度 | 根据实际 | 1m/2m/3m |

## 代码详解

### 雷达初始化

```c
// 1. 芯片上电
uapi_radar_set_power_status(POWERON);
osal_msleep(100);

// 2. 配置硬件参数
radar_hardware_para_t hw_para = {
    .channel_index = 0,
    .tx_power      = 31,
    .antenna_code  = {0, 1, 2},
    .agc_gain      = 50,
    .frame_interval = 100,  // 100ms 一帧
};
uapi_radar_set_hardware_para(&hw_para);

// 3. 配置算法参数
radar_alg_para_t alg_para = {
    .report_mode     = REPORT_MODE_ALL,
    .sensitivity     = 5,       // 灵敏度 1~10
    .range_boundary  = 700,     // 检测范围 cm
    .exit_delay      = 20,      // 退出延迟 s
    .ai_interference = 1,       // AI 抗干扰
};
uapi_radar_set_alg_para(&alg_para);
```

### 结果回调

```c
uapi_radar_register_result_cb(radar_result_cb);

static void radar_result_cb(radar_result_t *result)
{
    printf("[Radar] target_count=%d, human=%d\n",
           result->target_count, result->is_human_presence);

    for (int i = 0; i < result->target_count; i++) {
        radar_target_info_t *t = &result->targets[i];
        printf("  target[%d]: range=%dcm, angle=%.2f, vel=%dcm/s\n",
               t->id, t->range, t->angle / 100.0, t->velocity);
    }
}
```

### 启动

```c
uapi_radar_set_status(START);
// 此后雷达按 frame_interval 周期性产生结果
// 结果通过 radar_result_cb 异步回调
```

---

