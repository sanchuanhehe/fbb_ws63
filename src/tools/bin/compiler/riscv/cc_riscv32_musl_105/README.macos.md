# WS63 HCC macOS 工具链

`cc_riscv32_musl_mac` 和 `cc_riscv32_musl_fp_mac` 提供 Apple Silicon 原生 HCC 编译器，默认目标分别为 `rv32imc/ilp32` 和 `rv32imfc/ilp32f`。

- GCC 7.3.0 HCC SPC105 源码来自提交 `6b11bb34dc7114796ee2d27da0c3ae350e8f8eb2`。[HiSpark GCC](https://gitcode.com/HiSpark/gcc)
- Binutils 2.38 HCC SPC105B010 源码来自提交 `9bdd41cadfefaced27f7d9fac0a301e0b72bdb59`。[HiSpark Binutils](https://gitcode.com/HiSpark/binutils)
- 目标头文件和运行库沿用 SDK 的 SPC105 工具链。Soft-float 的静态 musl 库来自同版 SDK 的 Windows 工具链，归档内为 RISC-V ELF32 soft-float 目标文件。
- 主机工具由 Apple Clang 构建，使用 SDK 的 Secure C 实现。许可证见 `macos-licenses/`。

使用方法见仓库的《在 macOS 上编译 WS63 固件》。
