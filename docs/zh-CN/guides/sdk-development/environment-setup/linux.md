---
title: 在 Linux 上配置 WS63 开发环境
doc_type: how-to
product: WS63
applies_to:
  sdk: 1.10.108
  host: Linux（Ubuntu）
verification_level: static
source_refs:
  - src/build/config/target_config/ws63/ws63.json
---

# 在 Linux 上配置 WS63 开发环境

使用 Linux 配置 WS63 开发环境时，根据实际场景选择一种配置方式。CLI 方式由工具自动准备 SDK、工具链和构建依赖；手动方式由用户分别安装和维护这些依赖。

## 准备条件

- Ubuntu 主机，使用 Bash 终端。
- 可以访问 CLI、SDK 和工具链下载源的网络。
- 安装系统软件包需要具有 sudo 权限的账户；CLI 和构建依赖安装使用普通用户账户。

<a name="ZH-CN_TOPIC_0000001823873925"></a>

## 选择配置方式

| 场景 | 适用情况 | 配置入口 |
| --- | --- | --- |
| 使用 CLI 配置 | 新环境，或希望自动准备 SDK、工具链和构建依赖 | [本节的 CLI 配置步骤](#cli-setup) |
| 手动配置 | 已有 SDK，或需要自行管理 Python、CMake 和构建依赖 | 本页“手动配置环境” |

<a name="cli-setup"></a>

## 场景一：使用 CLI 配置

### 安装 CLI

更新软件包索引并安装系统工具：

```bash
sudo apt-get update
```

```bash
sudo apt-get install -y curl ca-certificates git make
```

安装 CLI：

```bash
curl -fsSL https://dl.hispark.hisilicon.com/bootstrap.sh | sh
```

重新打开终端后检查版本：

```bash
fbb -V
```

WS63 SDK 要求 `fbb` 不低于 `1.1.0`。

### 获取 SDK 和准备工具链

使用 CLI 获取 SDK：

```bash
fbb sdk install ws63
```

已有 SDK 时，进入 SDK 的 `src` 目录，将 `<SDK安装目录>` 替换为实际路径：

```bash
cd "<SDK安装目录>/src"
```

准备工具链和构建依赖：

```bash
fbb setup --sdk-dir .
```

检查环境：

```bash
fbb doctor
```

```bash
fbb describe --json
```


<a name="manual-setup"></a>

## 场景二：手动配置环境

以下适用于已有 SDK、需要自行管理构建依赖的环境。使用 Bash，准备 CMake 3.14.1 以上和 Python 3.8 以上版本。

### 安装 CMake<a name="ZH-CN_TOPIC_0000001823993865"></a>

在 Linux 终端安装 CMake：

```bash
sudo apt install cmake
```

### 安装 Python 环境<a name="ZH-CN_TOPIC_0000001777394014"></a>

安装 Python、pip 和虚拟环境支持：

```bash
sudo apt-get install -y python3 python3-pip python3-venv
```

检查 Python 版本：

```bash
python3 -V
```

版本不满足要求时，参考 [Python 安装指南](https://wiki.python.org/moin/BeginnersGuide/Download)。

创建专用虚拟环境：

```bash
python3 -m venv ~/.venvs/ws63
```

激活环境，每次打开新的构建终端时执行：

```bash
source ~/.venvs/ws63/bin/activate
```

安装 Kconfig 和签名工具使用的 Python 依赖：

```bash
python -m pip install setuptools "kconfiglib>=14.1,<15" "pycparser>=2.21,<3"
```

### 检查配置

检查 CMake：

```bash
cmake --version
```

检查 Python 依赖，正常时无输出且退出码为 `0`：

```bash
python -c "import kconfiglib, pycparser"
```

确认 Python 和 CMake 满足版本要求。依赖导入失败时，检查当前终端是否已激活上述虚拟环境。
