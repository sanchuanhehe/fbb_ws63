---
title: 在 macOS 上编译 WS63 固件
doc_type: how-to
product: WS63
applies_to:
  sdk: 1.10.108
  host: macOS（Apple Silicon）
verification_level: build
source_refs:
  - src/build.py
  - src/build/toolchains/riscv32_musl_105.cmake
  - src/build/toolchains/riscv32_musl_105_fp.cmake
---

# 在 macOS 上编译 WS63 固件

SDK 在 Apple Silicon 上自动选择随仓库提供的 HCC 7.3.0 工具链。开发机需要 Python 3、CMake（3.14.1 或更新版本）、Ninja 和支持 brainpoolP256r1 的 OpenSSL。

## 准备环境

使用 Homebrew 安装构建工具，并在 Python 虚拟环境中安装依赖。

```bash
brew install cmake ninja openssl@3
python3 -m venv ~/.venvs/ws63
source ~/.venvs/ws63/bin/activate
python -m pip install kconfiglib pyelftools 'pycparser<3'
```

## 配置签名密钥

沿用目标设备所用的密钥。首次验证工具链时，可以在隔离的 SDK 副本中生成测试密钥。

```bash
cd src/build/config/target_config/ws63/sign_config
umask 077
bash gen_all_key.sh sec
```

## 构建应用和生成固件包

在 SDK 仓库根目录运行以下命令。

```bash
cd src
python build.py ws63-liteos-app -ninja -nhso
```

构建使用原生 HCC 编译并生成 ELF/BIN。SDK 在 macOS 上使用 Python 和 OpenSSL 处理镜像、导出根公钥并生成 `.fwpkg`。应用使用 `rv32imfc/ilp32f`，Flashboot 和 Loaderboot 使用 `rv32imc/ilp32`。整包与应用烧写包位于 `src/output/ws63/fwpkg/ws63-liteos-app/`。

SDK 默认配置的 `SignSuite=4` 生成 SHA-256 校验镜像。配有对应设备密钥并启用 `SignSuite=0` 时，生成 BrainpoolP256r1 ECDSA 签名。仅需编译和链接时，可以给 `build.py` 添加 `-unsigned`。

可使用 SDK 的正常 OTA 命令生成升级文件。

```bash
python build/config/target_config/ws63/build_ws63_update.py --pkt app,nv --ver 1.0.0
```

加密生产流程使用与目标硬件对应的密钥、盐和 IV。原生实现支持 SDK 的 AES-128-CTR Flash/OTA 镜像。扩展双密钥配置会明确报错，生产密钥派生和设备烧录在本指南范围之外。
