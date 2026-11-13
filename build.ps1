param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug'
)

$ErrorActionPreference = 'Stop'
$bundleRoot = Join-Path $env:LOCALAPPDATA 'stm32cube/bundles'
$originalPath = $env:PATH

try {
    foreach ($tool in @('cmake', 'ninja', 'gnu-tools-for-stm32')) {
        $bundle = Get-ChildItem -LiteralPath (Join-Path $bundleRoot $tool) -Directory |
            Sort-Object LastWriteTime -Descending | Select-Object -First 1
        if (!$bundle) { throw "Missing STM32Cube bundle: $tool" }
        $env:PATH = (Join-Path $bundle.FullName 'bin') + ';' + $env:PATH
    }

    Push-Location (Join-Path $PSScriptRoot 'DaughterBoardFirmware')
    try {
        & cmake --preset $Configuration
        if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
        & cmake --build --preset $Configuration
        if ($LASTEXITCODE -ne 0) { throw 'Firmware build failed.' }
    }
    finally { Pop-Location }
}
finally { $env:PATH = $originalPath }
