# SLE 连接参数调优

本案例使用两块 WS63 开发板演示 SLE 连接建立后的参数更新。Server 发起更新，Client 输出收到的请求，双方输出最终生效参数。

## 目录

```text
sle_conn_param_tuning/
├── CMakeLists.txt
├── Kconfig
├── README.md
├── sle_conn_param_tuning.c
├── sle_conn_param_tuning_server/
│   ├── CMakeLists.txt
│   └── src/
└── sle_conn_param_tuning_client/
    ├── CMakeLists.txt
    └── src/
```

## 参数模式

连接间隔单位为 0.25 ms，监管超时单位为 10 ms。

| 模式 | 配置值 | 实际间隔 | Latency | 监管超时 |
|---|---:|---:|---:|---:|
| 低功耗 | 400 | 100 ms | 49 | 12 s |
| 平衡（默认） | 50 | 12.5 ms | 0 | 5 s |
| 低延迟 | 30 | 7.5 ms | 0 | 2 s |

低功耗模式使用 12 秒监管超时，以满足严格大于 `2 × (Latency + 1) × interval` 的约束。

## 编译

Server：

```shell
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_SERVER_SAMPLE=y --target ws63-liteos-app
fbb config set CONFIG_SLE_CONN_PARAM_PROFILE_BALANCED=y --target ws63-liteos-app
fbb build ws63-liteos-app --clean
```

Client：

```shell
fbb config set CONFIG_SAMPLE_SUPPORT_SLE_CONN_PARAM_TUNING_CLIENT_SAMPLE=y --target ws63-liteos-app
fbb build ws63-liteos-app --clean
```

## 运行与验收

1. 将 Server 和 Client 固件分别烧录到两块 WS63 开发板。
2. 先启动 Server，再启动 Client。
3. Server 应输出 `update request sent, status=0x0`。
4. Client 应输出 `update requested`。
5. 双端均应输出 `update complete`，且 `status=0x0` 和参数值与所选模式一致。

当前 SDK 没有 `sle_connect_param_update_rsp()`。`connect_param_update_req_cb` 用于观察请求，更新结果由 `connect_param_update_cb` 报告。
