@echo off
rem 批处理环境激活脚本
setlocal enabledelayedexpansion

echo ==== 激活开发环境 ====

rem 获取脚本所在目录
set SCRIPT_DIR=%~dp0

rem 激活虚拟环境
if exist "%SCRIPT_DIR%.venv\Scripts\activate.bat" (
    call "%SCRIPT_DIR%.venv\Scripts\activate.bat"
    echo ✓ 虚拟环境已激活
) else if exist "%SCRIPT_DIR%.venv\bin\activate" (
    rem WSL环境下的虚拟环境
    where bash >nul 2>&1
    if !errorlevel! equ 0 (
        bash -c "source ./.venv/bin/activate"
        echo ✓ WSL虚拟环境已激活
    ) else (
        echo ❌ 虚拟环境未找到，请先运行 config.bat 初始化环境
        pause
        exit /b 1
    )
) else (
    echo ❌ 虚拟环境未找到，请先运行 config.bat 初始化环境
    pause
    exit /b 1
)

rem 设置环境变量
set LC_ALL=C.UTF-8

rem 添加工具链到PATH
set TOOLS_PATH=%SCRIPT_DIR%tools\bin\compiler\riscv\cc_riscv32_musl_b090\cc_riscv32_musl_fp\bin
if exist "%TOOLS_PATH%" (
    set PATH=%TOOLS_PATH%;%PATH%
    echo ✓ 工具链路径已添加
) else (
    echo ⚠️  工具链路径不存在，某些功能可能无法正常工作
)

echo ✓ 开发环境已激活
echo 使用 'deactivate' 命令退出虚拟环境
