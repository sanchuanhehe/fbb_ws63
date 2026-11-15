# SLE RSSI 测距案例

本案例使用两块 WS63 开发板演示基于连接态 RSSI 的粗粒度测距。Server 负责广播并接受连接；Client 精确匹配广播名称，连接后每秒读取一次 RSSI，依次经过 7 点中值滤波、指数移动平均（EMA）和对数距离路径损耗模型，输出估算距离及 `near`、`middle`、`far` 分区。Client 还支持 GPIO13 长按触发的 100 cm 一点校准，并将结果保存到 NV。

> RSSI 测距容易受到遮挡、多径、天线方向和设备差异影响，只适合接近检测、趋势观察和粗粒度分区，不能替代 HADM 等高精度测距技术。

## 目录结构

```text
sle_rssi_ranging/
├── CMakeLists.txt
├── Kconfig
├── README.md
├── sle_rssi_ranging.c
├── sle_rssi_ranging_client/
│   ├── CMakeLists.txt
│   └── src/
│       ├── sle_rssi_ranging_calibration.c
│       ├── sle_rssi_ranging_calibration.h
│       ├── sle_rssi_ranging_client.c
│       └── sle_rssi_ranging_client.h
└── sle_rssi_ranging_server/
    ├── CMakeLists.txt
    └── src/
        ├── sle_rssi_ranging_server.c
        ├── sle_rssi_ranging_server.h
        ├── sle_rssi_ranging_server_adv.c
        └── sle_rssi_ranging_server_adv.h
```

## Kconfig 选项

- Server：`CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_SERVER_SAMPLE=y`
- Client：`CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_CLIENT_SAMPLE=y`
- 1 米参考 RSSI：`CONFIG_SLE_RSSI_RANGING_RSSI_AT_1M=-45`
- 路径损耗指数乘以 10：`CONFIG_SLE_RSSI_RANGING_PATH_LOSS_TENTHS=20`，即 `n=2.0`

Server 和 Client 位于同一个 Kconfig `choice` 中，应分别构建并烧录。

## 一键校准

校准硬件采用 HiHope WS63E 核心板的板载资源：

- GPIO13：低电平按下，内部上拉；长按 2 秒触发校准。
- GPIO5：连接 SK6805-EC20 可寻址 RGB 灯，24 位数据按 GRB 顺序发送。
- 默认及空闲状态：三色灯熄灭。
- 蓝灯闪烁：正在采集。
- 绿灯亮 3 秒后熄灭：计算完成且 NV 保存成功。
- 红灯亮 3 秒后熄灭：未连接、校准中断或 NV 保存失败。

SK6805-EC20 会锁存最后一次颜色，MCU 复位不会直接清除。Client 启动时仅保持 GPIO5 低电平，避免 SLE 启动和建链期间的时钟切换造成误码；建链稳定约 500 ms 后发送两次全黑帧，自动清除复位前可能残留的颜色。

使用方法：

1. 保持两块板连接，将 Server 和 Client 固定在 100 cm，使用与实际部署一致的天线方向。
2. 长按 Client 的 GPIO13 按键约 2 秒，看到蓝灯闪烁后松开。
3. Client 以 200 ms 间隔采集 31 个原始 RSSI，约 6.2 秒完成。
4. 取 31 个样本的中位数作为 `A=RSSI(1m)`，同时计算 MAD 和最小/最大值。
5. 将带 magic、版本和校验和的记录写入用户 NV `0x5101`；成功后绿灯亮 3 秒，随后发送两次全黑帧并熄灭。
6. 当前会话立即使用新值；后续正常重启会从 NV 加载。重新烧录全量 NV 镜像可能清除该值。

典型校准日志：

```text
[sle rssi cal] long press detected, calibration start: distance=100 cm, samples=31
[sle rssi cal] recording: 5/31, rssi=-52 dBm
[sle rssi cal] calibration complete: A=-52 dBm, MAD=1 dB, range=[-54,-50] dBm, samples=31, nv=ok
```

100 cm 单点只能校准固定偏移 `A`，不能同时求出路径损耗指数 `n`。若要拟合 `n`，至少还需要一个已知距离点，推荐使用多个距离点做线性回归。

## 算法

Client 使用如下对数距离路径损耗模型：

```text
d = 1 m × 10 ^ ((A - RSSI_filtered) / (10 × n))
```

其中 `A` 是 1 米处标定的 RSSI，`n` 是环境路径损耗指数。测距前先用 7 点中值滤波抑制突发异常值，再用 EMA 平滑短时抖动。默认分区为：

- `near`：不大于 150 cm
- `middle`：150～500 cm
- `far`：大于 500 cm

## 构建与验证

在 SDK 根目录分别构建两种角色：

```shell
fbb config set CONFIG_ENABLE_BT_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_SERVER_SAMPLE=y --target ws63-liteos-app
fbb build ws63-liteos-app --clean

fbb config set CONFIG_ENABLE_BT_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_RSSI_RANGING_CLIENT_SAMPLE=y --target ws63-liteos-app
fbb build ws63-liteos-app --clean
```

Client 的典型输出如下：

```text
[sle rssi client] found sle_rssi_server, scan_rssi=-37 dBm, stop seek
[sle rssi client] connected, conn_id=0x00, calibration=-45 dBm@1m, path_loss=2.0
[sle rssi client] range: raw=-36 dBm, median=-36 dBm, filtered=-36.0 dBm, samples=7, distance=35 cm, zone=near
```

`35 cm` 是默认教学标定参数下的模型估算值，不是独立测量得到的真实距离。
