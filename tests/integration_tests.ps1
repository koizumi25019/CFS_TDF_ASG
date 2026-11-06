$ErrorActionPreference = 'Stop'
Set-Location (Split-Path $PSScriptRoot -Parent)
& "$PSScriptRoot\run_tests.ps1"
$suffix = Get-Date -Format yyyyMMdd_HHmmss
$cases = @(
    @{ name='off'; options=@('-power','OFF','-nece','./input/necessary/s27_nece.txt') },
    @{ name='on'; options=@('-power','ON','-nece','./input/necessary/s27_nece.txt') },
    @{ name='all'; options=@('-power','ON','-nece','./input/necessary/s27_nece.txt','-cps','ON','-fp','ON','-resolving','ON','-sttg_wsath','ON') },
    @{ name='regen'; options=@('-power','ON','-nece','./input/necessary/s27_nece.txt','-regen','ON','-wsath','20') },
    @{ name='parallel'; options=@('-power','OFF','-mode_1','ON','-parallel','2') }
)
foreach ($case in $cases) {
    $folder = "./audit_$($case.name)_$suffix/"
    $arguments = @('-net','./input/circuit/s27.v','-fault','./input/rprf/s27_rpr_1.txt',
        '-seed','seed.txt','-xor_tap','./input/xor_tap/tap_4.txt',
        '-scan_chain','./input/scan_chain/s27_sc_4.txt','-target_limit','2',
        '-time','5','-folder',$folder) + $case.options
    & .\audit_build\TDF_ASG.exe @arguments *> "audit_build\integration_$($case.name).log"
    if ($LASTEXITCODE -ne 0) { throw "Integration failed: $($case.name)" }
    $problem = Get-Content "${folder}mttg_pbo.txt"
    $constraints = @($problem | Where-Object { $_ -notmatch '^\*|^min:' } | Select-String ';').Count
    $headerCount = [regex]::Match($problem[0], '#constraint= (\d+)').Groups[1].Value
    if ($constraints -ne [int]$headerCount) { throw 'Constraint header mismatch.' }
    $seeds = @(Get-Content "${folder}seed.txt").Count
    $log = Get-Content "${folder}log.txt" -Raw
    if ($log -notmatch "Number of Seeds\s+:$seeds\b") { throw 'Seed count mismatch.' }
    Write-Output "PASS: $($case.name), constraints=$constraints, seeds=$seeds"
}
& .\audit_build\TDF_ASG.exe -net *> audit_build\missing_option.log
if ($LASTEXITCODE -eq 0) { throw 'Missing option value was accepted.' }
& .\audit_build\TDF_ASG.exe -net ./input/circuit/s27.v -fault ./input/rprf/s27_rpr_1.txt -seed seed.txt `
    -xor_tap ./input/xor_tap/tap_4.txt -scan_chain ./input/scan_chain/s27_sc_4.txt -power ON `
    -folder "./audit_conflict_$suffix/" -regen ON -resolving ON *> audit_build\regen_conflict.log
if ($LASTEXITCODE -eq 0) { throw '-regen with -resolving was accepted.' }
