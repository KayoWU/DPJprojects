param(
    [string]$Root = ".",
    [string]$KeilUv4 = "E:\Keil5\UV4\UV4.exe"
)

$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path $Root).Path
$projectPath = Join-Path $resolvedRoot "UART.uvproj"
$buildLog = Join-Path $resolvedRoot "Objects\codex_build.log"
$checkScript = Join-Path $resolvedRoot "Tools\Check_AppLayer.ps1"

Write-Host "Running AUTOSAR-lite stage regression..."
Write-Host "Root: $resolvedRoot"

Write-Host ""
Write-Host "Step 1/2: App layer static check"
powershell -NoProfile -ExecutionPolicy Bypass -File $checkScript -Root $resolvedRoot

Write-Host ""
Write-Host "Step 2/2: Keil C51 build"
if (-not (Test-Path $KeilUv4)) {
    Write-Host "[FAIL] Keil UV4 not found: $KeilUv4"
    exit 1
}

& $KeilUv4 -b $projectPath -j0 -o $buildLog

if (-not (Test-Path $buildLog)) {
    Write-Host "[FAIL] Build log not found: $buildLog"
    exit 1
}

$logText = [System.IO.File]::ReadAllText($buildLog)
if ($logText -notmatch "0 Error\(s\), 0 Warning\(s\)") {
    Write-Host "[FAIL] Keil build did not finish cleanly."
    Write-Host $logText
    exit 1
}

Write-Host "[OK]   Keil build finished with 0 Error(s), 0 Warning(s)."
Write-Host ""
Write-Host "Stage regression passed."
