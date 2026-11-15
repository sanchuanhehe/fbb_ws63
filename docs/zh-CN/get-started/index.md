---
title: 快速入门
doc_type: tutorial
product: WS63
applies_to:
  sdk: 1.10.108
  target: ws63-liteos-app
  board: ws63
owner: 待确认
verification_level: static
source_refs:
  - src/build/config/target_config/ws63/ws63.json
  - src/application/samples/peripheral/helloworld/
  - src/application/samples/peripheral/Kconfig
---

# 快速入门

第一次使用 WS63 SDK 时，先选择一种开发方式，再沿对应页面完成环境准备、配置、构建、烧录和串口验证。两种方式使用相同的 SDK、Target、Kconfig 配置和 Hello World 完成判据。

## 适用范围

| 项目 | 要求 |
| --- | --- |
| SDK | `1.10.108` 及后续版本 |
| Target | `ws63-liteos-app` |
| 开发板 | [BearPi-Pico H3863 开发板](board-introduction.md#bearpi-pico-h3863) |
| 示例 | `SAMPLE_SUPPORT_HELLOWORLD` |
| 完成判据 | 串口出现 `start helloworld sample`，并持续输出 `hello world` |

## 选择开发方式

选择一种方式完成首次开发，不需要同时配置两套环境。

| 开发方式 | 适用场景 | 入口 |
| --- | --- | --- |
| CLI | 使用 Windows PowerShell 或 Linux Bash，命令可用于本地开发和 CI | [使用 CLI 完成第一次开发](cli/index.md) |
| VS Code 插件 | 使用 Windows 图形界面完成配置、构建、烧录和串口监视 | [使用 VS Code 完成第一次开发](vscode/index.md) |

如果需要其他开发环境的安装支持，请查看[配置 WS63 开发环境](../guides/sdk-development/environment-setup/index.md)。

## 完成判据

完成所选教程后，应同时满足：

- 生成 `ws63-liteos-app` 对应的 ELF 和完整固件包；
- 完整固件烧录成功；
- 串口日志出现 `start helloworld sample`；
- 串口持续输出 `hello world`。

本入口和两个教程当前只完成源码、配置和文档静态核对；目标板端到端验证完成后，再更新页面验证等级。
