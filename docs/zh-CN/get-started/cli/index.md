---
title: 使用 CLI 完成第一次开发
doc_type: tutorial
product: WS63
applies_to:
  target: ws63-liteos-app
  board: ws63
owner: 待确认
verification_level: static
source_refs:
  - src/build/config/target_config/ws63/ws63.json
  - src/application/samples/peripheral/helloworld/
  - src/application/samples/peripheral/Kconfig
  - src/build/config/target_config/ws63/config.py
  - src/tools/pkg/chip_packet/ws63/packet.py
upstream_refs:
  - https://gitcode.com/HiSpark/hs-fbb-cli
---

# 使用 CLI 完成第一次开发

本教程使用 FBB CLI，在 Windows PowerShell 或 Linux Bash 中完成 WS63 Hello World。两种系统使用相同的 SDK、Target、Kconfig 配置、固件格式和完成判据；命令壳层和串口名称按系统选择。

## 选择主机系统

在本页每组标签中选择所用系统；相同标签会同步切换。

=== "Windows"

    使用 Windows PowerShell 执行命令。

=== "Linux"

    使用 Linux Bash 执行命令。WSL 2 使用本页 Linux 命令。

## 前置条件

- 已按 [Git 官方安装说明](https://git-scm.com/downloads/) 和 [Git LFS 官方安装说明](https://git-lfs.com/)安装 Git 与 Git LFS，且 `git --version`、`git lfs version` 可以正常输出版本。
- 具有安装工具和使用串口的权限。
- 有足够空间保存 SDK、工具链和构建输出，工作目录不含中文字符和空格。
- 已准备 [BearPi-Pico H3863 开发板](../board-introduction.md#bearpi-pico-h3863) 和可传输数据的 Type-C USB 数据线。

## 1. 获取 SDK

在终端进入用于保存 SDK 的工作目录，执行以下命令：

```bash
git lfs install
```

```bash
git clone --branch master --single-branch https://gitcode.com/HiSpark/fbb_ws63.git
```

```bash
cd fbb_ws63
```

```bash
git lfs pull
```

进入 SDK 的 `src` 目录：

```bash
cd src
```

预期结果：Git LFS 文件下载完成，当前目录包含 `build.py`。如果 LFS 文件仍为指针文件，不要继续构建，先重新执行 `git lfs pull`。

## 2. 准备 CLI 和工具链

按所用系统执行对应的 CLI 安装命令。已经安装 CLI 时可跳过安装命令。

=== "Windows"

    在 PowerShell 中执行：

    ```powershell
    irm https://dl.hispark.hisilicon.com/bootstrap.ps1 | iex
    ```

=== "Linux"

    在 Bash 中执行：

    ```bash
    curl -fsSL https://dl.hispark.hisilicon.com/bootstrap.sh | sh
    ```

详细安装说明和手动配置方式见[配置 WS63 开发环境](../../guides/sdk-development/environment-setup/index.md)。WS63 SDK 要求 `fbb` 不低于 `1.1.0`。

检查 CLI：

```bash
fbb -V
```

在 SDK 的 `src` 目录准备工具链和构建依赖：

```bash
fbb setup --sdk-dir .
```

检查环境：

```bash
fbb doctor
```

查看 SDK、Target 和工具链识别结果：

```bash
fbb describe --json
```

预期结果：`fbb doctor` 成功退出，`fbb describe --json` 能识别 `ws63`、`ws63-liteos-app` 和 WS63 所需工具链。

## 3. 配置 Hello World

确认当前配置文件没有未保存的本地修改：

```bash
git diff --exit-code -- build/config/target_config/ws63/menuconfig/acore/ws63_liteos_app.config
```

使用 CLI 设置 Hello World 配置：

```bash
fbb config --target ws63-liteos-app set CONFIG_SAMPLE_ENABLE=y
```

```bash
fbb config --target ws63-liteos-app unset CONFIG_ENABLE_BT_SAMPLE
```

```bash
fbb config --target ws63-liteos-app set CONFIG_ENABLE_PERIPHERAL_SAMPLE=y
```

```bash
fbb config --target ws63-liteos-app set CONFIG_SAMPLE_SUPPORT_HELLOWORLD=y
```

检查配置值：

```bash
fbb config --target ws63-liteos-app get CONFIG_SAMPLE_ENABLE
```

```bash
fbb config --target ws63-liteos-app get CONFIG_ENABLE_BT_SAMPLE
```

```bash
fbb config --target ws63-liteos-app get CONFIG_ENABLE_PERIPHERAL_SAMPLE
```

```bash
fbb config --target ws63-liteos-app get CONFIG_SAMPLE_SUPPORT_HELLOWORLD
```

预期结果依次为 `y`、`n`、`y`、`y`。如果结果不同，重新执行对应的 `set` 或 `unset` 命令；不要直接编辑生成的 `.config` 文件。

## 4. 构建固件

配置改变后执行干净构建：

```bash
fbb build --clean ws63-liteos-app
```

### 构建较慢时

首次执行完整构建时会编译目标及其依赖，耗时可能较长。保持终端运行，等待命令完成，不要在构建过程中中断命令。

预期结果：命令以 `0` 退出，并生成以下两个文件：

```text
output/ws63/acore/ws63-liteos-app/ws63-liteos-app.elf
```

```text
output/ws63/fwpkg/ws63-liteos-app/ws63-liteos-app_all.fwpkg
```

检查文件是否生成：

=== "Windows"

    ```powershell
    Test-Path .\output\ws63\acore\ws63-liteos-app\ws63-liteos-app.elf
    ```

    ```powershell
    Test-Path .\output\ws63\fwpkg\ws63-liteos-app\ws63-liteos-app_all.fwpkg
    ```

=== "Linux"

    ```bash
    test -s output/ws63/acore/ws63-liteos-app/ws63-liteos-app.elf && echo "ELF: OK"
    ```

    ```bash
    test -s output/ws63/fwpkg/ws63-liteos-app/ws63-liteos-app_all.fwpkg && echo "FWPKG: OK"
    ```

## 5. 连接开发板并确认串口

使用 Type-C USB 数据线连接开发板，关闭占用串口的其他程序。

=== "Windows"

    在 PowerShell 中列出串口：

    ```powershell
    [System.IO.Ports.SerialPort]::GetPortNames()
    ```

    记录开发板对应的端口，例如 `COM3`。

=== "Linux"

    查看串口设备：

    ```bash
    ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
    ```

    记录开发板对应的设备路径，例如 `/dev/ttyUSB0`。

## 6. 烧录完整固件

烧录会覆盖开发板中的现有固件。需要保留原固件时，先取得可恢复的固件包。

将 `<PORT>` 替换为实际端口，在 SDK 的 `src` 目录执行：

```bash
fbb flash ws63-liteos-app --port <PORT> --baud 921600 --manual-reset
```

工具提示连接设备时，按下开发板的 `RST` 按钮。预期结果：命令正常退出，并报告 `All images burnt successfully` 或等价成功结果。

## 7. 监视并验证 Hello World

将 `<PORT>` 替换为实际端口：

```bash
fbb monitor --chip ws63 --port <PORT> --baud 115200
```

打开监视器后按一次开发板的 `RST` 按钮。成功日志必须包含：

```text
start helloworld sample
```

```text
hello world
```

随后应约每秒重复输出 `hello world`。只看到系统启动日志不能判定 Hello World 成功。验证完成后按 `Ctrl+C` 退出监视器。

## 完成判据

- FBB CLI、工具链和 `ws63-liteos-app` Target 通过环境检查；
- ELF 和完整固件包均由本次构建生成；
- 完整固件以 `921600` 波特率烧录成功；
- 串口以 `115200` 波特率看到启动标志和重复的 `hello world`。

目标板端到端验证完成后，再将页面验证等级更新为 `verified`。
