param(
    [string]$Root = "."
)

$ErrorActionPreference = "Stop"
$failed = $false

function Test-Condition {
    param(
        [bool]$Condition,
        [string]$Message
    )

    if ($Condition) {
        Write-Host "[OK]   $Message"
    }
    else {
        Write-Host "[FAIL] $Message"
        $script:failed = $true
    }
}

function Get-Text {
    param([string]$Path)
    return [System.IO.File]::ReadAllText((Join-Path $Root $Path))
}

$appPath = "App\App.c"
$uvprojPath = "UART.uvproj"
$rteHeaderPath = "Bsw\Service\Rte\Rte.h"
$swcDir = Join-Path $Root "App\Swc"

$appText = Get-Text $appPath
$uvprojText = Get-Text $uvprojPath
$rteText = Get-Text $rteHeaderPath

$expectedSwcs = @(
    "Swc_AdcSampler",
    "Swc_Command",
    "Swc_Storage",
    "Swc_IrHandler",
    "Swc_LcdPresenter",
    "Swc_SerialTx"
)

Write-Host "Checking AUTOSAR-lite App/SWC/RTE structure..."

foreach ($swc in $expectedSwcs) {
    Test-Condition (Test-Path (Join-Path $swcDir "$swc.c")) "Found App\Swc\$swc.c"
    Test-Condition (Test-Path (Join-Path $swcDir "$swc.h")) "Found App\Swc\$swc.h"
    Test-Condition ($uvprojText.Contains(".\App\Swc\$swc.c")) "UART.uvproj includes $swc.c"
}

Test-Condition ($uvprojText.Contains(".\App\Swc")) "UART.uvproj include path contains .\App\Swc"

$forbiddenDriverIncludes = @(
    'Serial',
    'Lcd1602',
    'At24c02',
    'Iic',
    'Adc',
    'IrSensor',
    'Led8'
)

foreach ($name in $forbiddenDriverIncludes) {
    $pattern = '#include\s+"' + $name + '\.h"'
    Test-Condition (-not [regex]::IsMatch($appText, $pattern)) "App.c does not include $name.h"
}

$forbiddenRegisters = @("SBUF", "P0", "P1", "P2", "P3")
foreach ($reg in $forbiddenRegisters) {
    $pattern = '\b' + $reg + '\b'
    Test-Condition (-not [regex]::IsMatch($appText, $pattern)) "App.c does not access $reg"
}

foreach ($swc in $expectedSwcs) {
    Test-Condition ($appText.Contains($swc + "_Init();")) "App.c initializes $swc"
    Test-Condition ([regex]::IsMatch($appText, $swc + '_MainFunction\([^;]*\);')) "App.c schedules $swc"
}

$swcFiles = Get-ChildItem -Path $swcDir -Filter "*.c"
foreach ($file in $swcFiles) {
    $text = [System.IO.File]::ReadAllText($file.FullName)
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($file.Name)

    foreach ($name in $forbiddenDriverIncludes) {
        $pattern = '#include\s+"' + $name + '\.h"'
        Test-Condition (-not [regex]::IsMatch($text, $pattern)) "$($file.Name) does not include $name.h"
    }

    $swcIncludes = [regex]::Matches($text, '#include\s+"(Swc_[^"]+\.h)"')
    foreach ($match in $swcIncludes) {
        $included = $match.Groups[1].Value
        Test-Condition ($included -eq ($baseName + ".h")) "$($file.Name) does not include another SWC header ($included)"
    }

    Test-Condition ($text.Contains('#include "Rte.h"')) "$($file.Name) uses Rte.h"
}

$requiredRtePorts = @(
    "Rte_WriteAdcSample",
    "Rte_ReadAdcRaw",
    "Rte_ReadAdcMv",
    "Rte_RequestAdcSave",
    "Rte_RequestAdcRead",
    "Rte_ConsumeAdcSaveRequest",
    "Rte_ConsumeAdcReadRequest",
    "Rte_DisplayRequestAdcLine",
    "Rte_DisplayConsumeAdcLine",
    "Rte_DisplayRequestStatusLine",
    "Rte_DisplayRequestReadLine",
    "Rte_DisplayRequestIrLine",
    "Rte_DisplayConsumeLine2",
    "Rte_RequestSerialReadMessage",
    "Rte_ConsumeSerialReadMessage"
)

foreach ($port in $requiredRtePorts) {
    Test-Condition ($rteText.Contains($port)) "Rte.h exposes $port"
}

if ($failed) {
    Write-Host "App layer static check failed."
    exit 1
}

Write-Host "App layer static check passed."
