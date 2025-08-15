# PowerShell 环境激活脚本
# 获取脚本所在目录的绝对路径
$SCRIPT_DIR = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

Write-Host "==== 激活开发环境 ====" -ForegroundColor Magenta

# 激活虚拟环境
$venvPath = Join-Path $SCRIPT_DIR ".venv"
$activateScript = Join-Path $venvPath "Scripts\Activate.ps1"

if (Test-Path $activateScript) {
    try {
        & $activateScript
        Write-Host "✓ 虚拟环境已激活" -ForegroundColor Green
    }
    catch {
        Write-Host "⚠️  虚拟环境激活失败，尝试WSL环境..." -ForegroundColor Yellow
        # WSL环境下的虚拟环境
        if (Get-Command bash -ErrorAction SilentlyContinue) {
            bash -c "source ./.venv/bin/activate"
            Write-Host "✓ WSL虚拟环境已激活" -ForegroundColor Green
        } else {
            Write-Host "❌ 虚拟环境激活失败，请先运行 config.ps1 初始化环境" -ForegroundColor Red
            exit 1
        }
    }
} elseif (Test-Path (Join-Path $venvPath "bin\activate")) {
    # WSL环境下的虚拟环境
    if (Get-Command bash -ErrorAction SilentlyContinue) {
        bash -c "source ./.venv/bin/activate"
        Write-Host "✓ WSL虚拟环境已激活" -ForegroundColor Green
    } else {
        Write-Host "❌ 虚拟环境未找到，请先运行 config.ps1 初始化环境" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "❌ 虚拟环境未找到，请先运行 config.ps1 初始化环境" -ForegroundColor Red
    exit 1
}

# 设置环境变量
$env:LC_ALL = "C.UTF-8"

# 添加工具链到PATH
$TOOLS_PATH = Join-Path $SCRIPT_DIR "tools\bin\compiler\riscv\cc_riscv32_musl_b090\cc_riscv32_musl_fp\bin"
if (Test-Path $TOOLS_PATH) {
    $env:PATH = "$TOOLS_PATH;$env:PATH"
    Write-Host "✓ 工具链路径已添加" -ForegroundColor Green
} else {
    Write-Host "⚠️  工具链路径不存在，某些功能可能无法正常工作" -ForegroundColor Yellow
}

Write-Host "✓ 开发环境已激活" -ForegroundColor Green
Write-Host "使用 'deactivate' 命令退出虚拟环境" -ForegroundColor Yellow
