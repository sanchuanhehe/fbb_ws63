---
title: 在 WSL 中配置 WS63 开发环境
doc_type: how-to
product: WS63
applies_to:
  sdk: 1.10.108
  target: ws63-liteos-app
  host: Windows + WSL 2（Ubuntu）
verification_level: static
source_refs:
  - src/build/config/target_config/ws63/target_config.py
  - src/build/toolchains/riscv32_musl_105_fp.cmake
  - docs/zh-CN/guides/sdk-development/environment-setup/linux.md
upstream_refs:
  - https://learn.microsoft.com/zh-cn/windows/wsl/install
  - https://learn.microsoft.com/zh-cn/windows/wsl/filesystems
  - https://learn.microsoft.com/zh-cn/windows/wsl/connect-usb
---

# 在 WSL 中配置 WS63 开发环境

在 Windows 主机的 WSL 2 Ubuntu 中使用 CLI 完成 SDK 配置、构建、烧录和串口调试。

## 准备条件

- Windows 10 版本 2004（内部版本 19041）及以上，或 Windows 11。
- 使用具有管理员权限的账户，并在 BIOS/UEFI 中启用硬件虚拟化。
- 具有稳定网络，可下载 Ubuntu、CLI、WS63 SDK 和 Linux 工具链。
- 已有 SDK 时，可直接接入现有 SDK。

## 配置 WSL 2

按微软的 [WSL 安装指南](https://learn.microsoft.com/zh-cn/windows/wsl/install)安装 Ubuntu，完成重启及 Linux 用户创建。

在 Windows PowerShell 中检查发行版和 WSL 版本：

```powershell
wsl --list --verbose
```

确认 Ubuntu 对应的 `VERSION` 为 `2`。版本转换方法见同一安装指南。

## 配置 Linux 构建环境

完成 WSL 2 配置后，打开 Ubuntu 终端，按[在 Linux 上配置 WS63 开发环境](linux.md)完成 CLI 或手动环境配置。WSL 使用 Linux 页面中的命令和步骤。


Windows 与 WSL 分别使用各自的 Python、CMake 和工具链，WSL 构建使用 Linux 版工具。

## SDK 存储位置

- 将 SDK 放在 Linux 用户目录下，例如 `~/ws63-sdk/`；使用 `fbb sdk install` 时，以安装输出的位置为准。源码和构建目录采用 WSL 的 Linux 文件系统，见微软的 [WSL 文件存储建议](https://learn.microsoft.com/zh-cn/windows/wsl/filesystems)。
- Linux 工具链由 WSL 中的 fbb 管理。
- Windows 和 WSL 使用独立的 SDK 工作副本及构建输出目录。

## 检查配置

确认 `fbb doctor` 检查通过，并识别到 WSL 中的 SDK 和构建工具。

- Windows 能识别开发板的 COM 口。

依赖或工具链缺失时，在 WSL 中按诊断提示修复；串口不可见时，检查 Windows 侧驱动和 USB 连接。
