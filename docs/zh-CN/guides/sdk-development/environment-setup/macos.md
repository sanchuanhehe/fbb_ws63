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

SDK 在 Apple Silicon 上自动选择随仓库提供的 HCC 7.3.0 工具链。开发机需要 Python 3、CMake（3.14.1 或更新版本）和 Ninja。

## 准备环境

使用 Homebrew 安装构建工具，并在 Python 虚拟环境中安装依赖。

```bash
brew install cmake ninja
python3 -m venv ~/.venvs/ws63
source ~/.venvs/ws63/bin/activate
python -m pip install kconfiglib pyelftools 'pycparser<3'
```

## 编译应用和引导程序

在 SDK 仓库根目录运行：

```bash
cd src
python build.py ws63-liteos-app -ninja
python build.py ws63-flashboot -ninja
```

构建结果位于 `src/output/ws63/acore/` 下对应的目标目录。应用使用 `rv32imfc/ilp32f` 工具链，Flashboot 使用 `rv32imc/ilp32` 工具链。

macOS 构建输出未签名的 ELF 和 BIN。固件签名、`.fwpkg` 打包和 HSO 数据库生成使用 Linux 或 Windows 构建流程。
