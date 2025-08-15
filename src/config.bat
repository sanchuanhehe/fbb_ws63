@echo off
rem 批处理配置脚本 - 完整环境初始化
setlocal enabledelayedexpansion

echo ==== Windows 开发环境配置脚本 ====

rem 获取脚本所在目录
set SCRIPT_DIR=%~dp0

rem 定义安装Python包的函数
goto :skip_functions

:install_python_package
set package=%1
set package_name=%package:>=% 
set package_name=%package_name:~=%
echo 安装 %package_name%...

for /L %%i in (1,1,3) do (
    echo 尝试第 %%i 次安装 %package_name%...
    pip install %package% --timeout=120 >nul 2>&1
    if !errorlevel! equ 0 (
        echo ✓ %package_name% 安装成功
        goto :eof
    ) else (
        echo ⚠️  %package_name% 安装失败，重试中...
        if %%i equ 3 (
            echo ❌ %package_name% 安装失败，请手动安装
            goto :eof
        )
        timeout /t 2 /nobreak >nul
    )
)
goto :eof

:skip_functions

echo ==== 检查系统依赖 ====

rem 检查Python
python --version >nul 2>&1
if !errorlevel! neq 0 (
    echo ❌ Python 未安装，请先安装 Python 3.8+
    echo    下载地址: https://www.python.org/downloads/
    pause
    exit /b 1
) else (
    for /f "tokens=*" %%a in ('python --version 2^>^&1') do set python_version=%%a
    echo ✓ !python_version! 已安装
)

rem 检查pip
pip --version >nul 2>&1
if !errorlevel! neq 0 (
    echo ❌ pip 未安装，请重新安装 Python 并确保包含 pip
    pause
    exit /b 1
) else (
    echo ✓ pip 已安装
)

rem 检查git
git --version >nul 2>&1
if !errorlevel! neq 0 (
    echo ❌ Git 未安装，请先安装 Git
    echo    下载地址: https://git-scm.com/downloads
    pause
    exit /b 1
) else (
    echo ✓ Git 已安装
)

echo ==== 设置Python虚拟环境 ====
set VENV_DIR=.venv
set VENV_PATH=%SCRIPT_DIR%%VENV_DIR%

if not exist "%VENV_PATH%" (
    echo 创建Python虚拟环境: %VENV_DIR%
    python -m venv "%VENV_PATH%"
    echo ✓ 虚拟环境已创建
) else (
    echo ✓ 虚拟环境已存在
)

rem 激活虚拟环境
if exist "%VENV_PATH%\Scripts\activate.bat" (
    call "%VENV_PATH%\Scripts\activate.bat"
    echo ✓ 虚拟环境已激活
) else (
    echo ❌ 虚拟环境激活脚本未找到
    pause
    exit /b 1
)

echo ==== 配置pip镜像源 ====
rem 创建pip配置目录
set PIP_DIR=%APPDATA%\pip
if not exist "%PIP_DIR%" (
    mkdir "%PIP_DIR%"
)

rem 配置pip使用清华镜像源
(
echo [global]
echo index-url = https://pypi.tuna.tsinghua.edu.cn/simple/
echo trusted-host = pypi.tuna.tsinghua.edu.cn  
echo timeout = 120
echo retries = 3
) > "%PIP_DIR%\pip.ini"

echo ✓ 已配置pip使用清华镜像源

rem 升级pip (带重试机制)
echo 升级pip...
for /L %%i in (1,1,3) do (
    echo 尝试第 %%i 次升级pip...
    python -m pip install --upgrade pip --timeout=120 >nul 2>&1
    if !errorlevel! equ 0 (
        echo ✓ pip升级成功
        goto :pip_upgraded
    ) else (
        echo ⚠️  pip升级失败，重试中...
        if %%i equ 3 (
            echo ⚠️  pip升级失败，使用当前版本继续
        )
        timeout /t 2 /nobreak >nul
    )
)
:pip_upgraded

echo ==== 安装Python依赖包 ====

rem 安装Python依赖包
call :install_python_package "pycparser>=2.21"
call :install_python_package "pyyaml"
call :install_python_package "kconfiglib"  
call :install_python_package "setuptools"

echo ==== 设置环境变量 ====

rem 设置环境变量
set LC_ALL=C.UTF-8

rem 添加工具链到PATH
set TOOLS_PATH=%SCRIPT_DIR%tools\bin\compiler\riscv\cc_riscv32_musl_b090\cc_riscv32_musl_fp\bin
if exist "%TOOLS_PATH%" (
    set PATH=%TOOLS_PATH%;%PATH%
    echo ✓ 工具链路径已添加
) else (
    echo ⚠️  工具链路径不存在: %TOOLS_PATH%
)

echo ==== 环境检查总结 ====
echo ✓ 系统依赖已检查
echo ✓ Python虚拟环境已创建: .venv
echo ✓ Python依赖包已安装
echo ✓ 环境变量已设置
echo.
echo 使用方法：
echo 1. 每次开发前运行: activate_env.bat
echo 2. 或手动激活: .venv\Scripts\activate.bat
echo.
echo 环境初始化完成！

pause
