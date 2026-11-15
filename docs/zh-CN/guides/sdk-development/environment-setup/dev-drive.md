---
title: Windows（开发者驱动器）
doc_type: how-to
product: WS63
applies_to:
  sdk: 1.10.108
  host: Windows 11
verification_level: static
source_refs:
  - docs/zh-CN/guides/sdk-development/environment-setup/windows.md
upstream_refs:
  - https://learn.microsoft.com/zh-cn/windows/dev-drive/
  - https://learn.microsoft.com/zh-cn/windows/wsl/filesystems
---

# Windows（开发者驱动器）

开发者驱动器（Dev Drive）是基于 ReFS 的 Windows 存储卷，用于存放 WS63 SDK、应用工程和构建产物。

## 准备条件

- Windows 11 版本及磁盘空间满足微软的 [Dev Drive 要求](https://learn.microsoft.com/zh-cn/windows/dev-drive/#先决条件)。
- 具有本地管理员权限；企业设备符合组织的存储与安全策略。
- 已备份原工程、未提交的修改及需要保留的配置。

## 创建开发盘

> 创建或调整磁盘卷前确认目标磁盘和备份。保留原 SDK 与工程，完成新路径检查后再处理旧副本；防病毒配置遵循组织的安全策略。

按微软的 [Dev Drive 设置指南](https://learn.microsoft.com/zh-cn/windows/dev-drive/)创建开发盘，已有开发盘可直接使用。下文以 `E:` 为例，请替换为实际盘符。

开发盘用于项目数据，VS Code、HiSpark Studio 和工具链保留原安装位置。

## 设置 SDK 与工程路径

1. 将 SDK 下载或复制到开发盘，并将应用工程放入独立目录，例如：

    ```text
    E:\ws63\
    ├── sdk\
    │   └── src\
    │       └── build.py
    └── projects\
    ```

2. 使用 HiSpark Studio 创建工程时，将“软件包”设为 `E:\ws63\sdk\src`，工程保存位置设为 `E:\ws63\projects` 下的独立目录。已有工程迁移后，重新打开新目录并检查工程配置中的 SDK 路径。
3. 在新路径下使用独立的构建输出目录重新构建，不复用旧路径的 CMake 缓存。重新选择生成的固件包、ELF 和其他调试产物。

## 检查配置

在 Windows PowerShell 中检查 SDK 入口文件，预期输出为 `True`：

```powershell
Test-Path -LiteralPath 'E:\ws63\sdk\src\build.py'
```

- HiSpark Studio 能打开开发盘上的工程，软件包路径指向新 SDK 的 `src` 目录。
- 构建完成，新目录中的产物时间戳已更新。
- 烧录和调试配置指向新构建产物。

若新路径不可写或工具无法使用该路径，保留日志，将工程和 SDK 路径恢复为原副本。

## WSL 存储位置

WSL 构建的源码和输出目录放在 Linux 文件系统中，例如 `/home/<用户名>/ws63-sdk/`。Dev Drive 不作为 WSL 构建的必选配置；微软对 WSL 项目文件的适用说明见 [Dev Drive 与 WSL](https://learn.microsoft.com/zh-cn/windows/dev-drive/#开发驱动器是否适用于-wsl-项目文件)。
