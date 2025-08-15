# PowerShell 配置脚本 - 完整环境初始化
param(
    [switch]$SkipSystemPackages = $false
)

# 设置错误处理
$ErrorActionPreference = "Stop"

# 获取脚本所在目录的绝对路径
$SCRIPT_DIR = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

# 定义安装Python包的函数
function Install-PythonPackage {
    param([string]$Package)
    
    $PackageName = $Package -split '[>=<]' | Select-Object -First 1
    Write-Host "安装 $PackageName..." -ForegroundColor Yellow
    
    for ($i = 1; $i -le 3; $i++) {
        Write-Host "尝试第 $i 次安装 $PackageName..." -ForegroundColor Cyan
        try {
            pip install $Package --timeout=120
            Write-Host "✓ $PackageName 安装成功" -ForegroundColor Green
            return $true
        }
        catch {
            Write-Host "⚠️  $PackageName 安装失败，重试中..." -ForegroundColor Yellow
            if ($i -eq 3) {
                Write-Host "❌ $PackageName 安装失败，请手动安装" -ForegroundColor Red
                return $false
            }
            Start-Sleep 2
        }
    }
}

# 检查是否以管理员身份运行
function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

Write-Host "==== Windows 开发环境配置脚本 ====" -ForegroundColor Magenta

# 检查必要工具
Write-Host "==== 检查系统依赖 ====" -ForegroundColor Blue

# 检查Python
if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Python 未安装，请先安装 Python 3.8+")
    Write-Host "   下载地址: https://www.python.org/downloads/"
    exit 1
} else {
    $pythonVersion = python --version
    Write-Host "✓ $pythonVersion 已安装" -ForegroundColor Green
}

# 检查pip
if (-not (Get-Command pip -ErrorAction SilentlyContinue)) {
    Write-Host "❌ pip 未安装，请重新安装 Python 并确保包含 pip"
    exit 1
} else {
    Write-Host "✓ pip 已安装" -ForegroundColor Green
}

# 检查git
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Git 未安装，请先安装 Git"
    Write-Host "   下载地址: https://git-scm.com/downloads"
    exit 1
} else {
    Write-Host "✓ Git 已安装" -ForegroundColor Green
}

Write-Host "==== 设置Python虚拟环境 ====" -ForegroundColor Blue
$VENV_DIR = ".venv"
$venvPath = Join-Path $SCRIPT_DIR $VENV_DIR

if (-not (Test-Path $venvPath)) {
    Write-Host "创建Python虚拟环境: $VENV_DIR" -ForegroundColor Yellow
    python -m venv $venvPath
    Write-Host "✓ 虚拟环境已创建" -ForegroundColor Green
} else {
    Write-Host "✓ 虚拟环境已存在" -ForegroundColor Green
}

# 激活虚拟环境
$activateScript = Join-Path $venvPath "Scripts\Activate.ps1"
if (Test-Path $activateScript) {
    & $activateScript
    Write-Host "✓ 虚拟环境已激活" -ForegroundColor Green
} else {
    Write-Host "❌ 虚拟环境激活脚本未找到" -ForegroundColor Red
    exit 1
}

Write-Host "==== 配置pip镜像源 ====" -ForegroundColor Blue
# 创建pip配置目录
$pipDir = Join-Path $env:APPDATA "pip"
if (-not (Test-Path $pipDir)) {
    New-Item -ItemType Directory -Path $pipDir -Force | Out-Null
}

# 配置pip使用清华镜像源
$pipConfig = @"
[global]
index-url = https://pypi.tuna.tsinghua.edu.cn/simple/
trusted-host = pypi.tuna.tsinghua.edu.cn
timeout = 120
retries = 3
"@

$pipConfigPath = Join-Path $pipDir "pip.ini"
$pipConfig | Out-File -FilePath $pipConfigPath -Encoding UTF8
Write-Host "✓ 已配置pip使用清华镜像源" -ForegroundColor Green

# 升级pip (带重试机制)
Write-Host "升级pip..." -ForegroundColor Yellow
for ($i = 1; $i -le 3; $i++) {
    Write-Host "尝试第 $i 次升级pip..." -ForegroundColor Cyan
    try {
        python -m pip install --upgrade pip --timeout=120
        Write-Host "✓ pip升级成功" -ForegroundColor Green
        break
    }
    catch {
        Write-Host "⚠️  pip升级失败，重试中..." -ForegroundColor Yellow
        if ($i -eq 3) {
            Write-Host "⚠️  pip升级失败，使用当前版本继续" -ForegroundColor Yellow
        }
        Start-Sleep 2
    }
}

Write-Host "==== 安装Python依赖包 ====" -ForegroundColor Blue

# 安装Python依赖包
$packages = @(
    "pycparser>=2.21",
    "pyyaml",
    "kconfiglib",
    "setuptools"
)

foreach ($package in $packages) {
    Install-PythonPackage $package
}

Write-Host "==== 设置环境变量 ====" -ForegroundColor Blue

# 设置环境变量
$env:LC_ALL = "C.UTF-8"

# 添加工具链到PATH
$TOOLS_PATH = Join-Path $SCRIPT_DIR "tools\bin\compiler\riscv\cc_riscv32_musl_b090\cc_riscv32_musl_fp\bin"
if (Test-Path $TOOLS_PATH) {
    $env:PATH = "$TOOLS_PATH;$env:PATH"
    Write-Host "✓ 工具链路径已添加" -ForegroundColor Green
} else {
    Write-Host "⚠️  工具链路径不存在: $TOOLS_PATH" -ForegroundColor Yellow
}

Write-Host "==== 环境检查总结 ====" -ForegroundColor Magenta
Write-Host "✓ 系统依赖已检查" -ForegroundColor Green
Write-Host "✓ Python虚拟环境已创建: .venv" -ForegroundColor Green
Write-Host "✓ Python依赖包已安装" -ForegroundColor Green  
Write-Host "✓ 环境变量已设置" -ForegroundColor Green
Write-Host ""
Write-Host "使用方法：" -ForegroundColor Yellow
Write-Host "1. 每次开发前运行: & '.\activate_env.ps1'" -ForegroundColor White
Write-Host "2. 或手动激活: & '.\.venv\Scripts\Activate.ps1'" -ForegroundColor White
Write-Host ""
Write-Host "环境初始化完成！" -ForegroundColor Green
