param([switch]$AddressSanitizer)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vc = Get-ChildItem "$vs\VC\Tools\MSVC" -Directory | Sort-Object Name -Descending | Select-Object -First 1
$sdk = Get-ChildItem "${env:ProgramFiles(x86)}\Windows Kits\10\Include" -Directory | Sort-Object Name -Descending | Select-Object -First 1
$kit = "${env:ProgramFiles(x86)}\Windows Kits\10"
$env:INCLUDE = "$($vc.FullName)\include;$($sdk.FullName)\ucrt;$($sdk.FullName)\shared;$($sdk.FullName)\um"
$env:LIB = "$($vc.FullName)\lib\x64;$kit\Lib\$($sdk.Name)\ucrt\x64;$kit\Lib\$($sdk.Name)\um\x64"
$configuration = 'Release'
$build = 'audit_build'
$compileFlags = @('/MD')
if ($AddressSanitizer) {
    $configuration = 'Debug'
    $build = 'audit_asan'
    $compileFlags = @('/MDd', '/fsanitize=address', '/Zi')
    $env:PATH = "$($vc.FullName)\bin\Hostx64\x64;$env:PATH"
}
& "$vs\MSBuild\Current\Bin\MSBuild.exe" TDF_ASG.vcxproj "/p:Configuration=$configuration" /p:Platform=x64 "/p:OutDir=$root\$build\" "/p:IntDir=$root\$build\obj\" /v:quiet /nologo
if ($LASTEXITCODE -ne 0) { throw 'Application build failed.' }
& "$($vc.FullName)\bin\Hostx64\x64\cl.exe" /nologo /c /TC @compileFlags tests\core_tests.c "/Fo:$build\core_tests.obj"
if ($LASTEXITCODE -ne 0) { throw 'Test compilation failed.' }
& "$($vc.FullName)\bin\Hostx64\x64\cl.exe" /nologo /c /TC @compileFlags tests\input_tests.c "/Fo:$build\input_tests.obj"
if ($LASTEXITCODE -ne 0) { throw 'Input test compilation failed.' }
$objects = Get-ChildItem "$build\obj\*.obj" | Where-Object Name -ne 'main.obj' | Select-Object -ExpandProperty FullName
& "$($vc.FullName)\bin\Hostx64\x64\link.exe" /nologo /LTCG /DEBUG "/out:$build\core_tests.exe" "$build\core_tests.obj" "$build\input_tests.obj" $objects
if ($LASTEXITCODE -ne 0) { throw 'Test link failed.' }
& ".\$build\core_tests.exe"
if ($LASTEXITCODE -ne 0) { throw 'Core tests failed.' }
