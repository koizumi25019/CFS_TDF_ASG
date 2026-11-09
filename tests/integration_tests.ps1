$ErrorActionPreference = 'Stop'
Set-Location (Split-Path $PSScriptRoot -Parent)
& "$PSScriptRoot\run_tests.ps1"
$suffix = Get-Date -Format yyyyMMdd_HHmmss
# lfsr=$false: -lfsr OFF (test patterns only; no -xor_tap / -scan_chain)
$cases = @(
    @{ name='off'; lfsr=$true; options=@('-power','OFF') },
    @{ name='on'; lfsr=$true; options=@('-power','ON') },
    @{ name='all'; lfsr=$true; options=@('-power','ON','-cps','ON','-fp','ON') },
    @{ name='regen'; lfsr=$true; options=@('-power','ON','-regen','ON','-wsath','20') },
    @{ name='parallel'; lfsr=$true; options=@('-power','OFF','-mode_1','ON','-parallel','2') },
    @{ name='untest'; lfsr=$true; options=@('-power','OFF','-untest_mode','ON') },
    @{ name='atpg'; lfsr=$false; options=@('-power','OFF','-lfsr','OFF') },
    @{ name='atpg_power'; lfsr=$false; options=@('-power','ON','-lfsr','OFF','-untest_mode','ON') }
)
foreach ($case in $cases) {
    $folder = "./audit_$($case.name)_$suffix/"
    $arguments = @('-net','./input/circuit/s27.v','-fault','./input/rprf/s27_rpr_1.txt',
        '-target_limit','2','-time','5','-folder',$folder) + $case.options
    if ($case.lfsr) {
        $arguments += @('-xor_tap','./input/xor_tap/tap_4.txt','-scan_chain','./input/scan_chain/s27_sc_4.txt')
    }
    & .\audit_build\TDF_ASG.exe @arguments *> "audit_build\integration_$($case.name).log"
    if ($LASTEXITCODE -ne 0) { throw "Integration failed: $($case.name)" }
    $problem = Get-Content "${folder}mttg_pbo.txt"
    $constraints = @($problem | Where-Object { $_ -notmatch '^\*|^min:' } | Select-String ';').Count
    $headerCount = [regex]::Match($problem[0], '#constraint= (\d+)').Groups[1].Value
    if ($constraints -ne [int]$headerCount) { throw 'Constraint header mismatch.' }
    $patterns = @(Get-Content "${folder}tp_set.txt").Count
    $log = Get-Content "${folder}log.txt" -Raw
    if ($log -notmatch "Number of Seeds\s+:$patterns\b") { throw 'Test pattern count mismatch.' }
    if ($case.lfsr) {
        # one seed per test pattern, in the same order
        if (@(Get-Content "${folder}seed.txt").Count -ne $patterns) { throw 'Seed count mismatch.' }
    }
    elseif (Test-Path "${folder}seed.txt") { throw 'seed.txt was written without the LFSR.' }
    Write-Output "PASS: $($case.name), constraints=$constraints, patterns=$patterns"
}
& .\audit_build\TDF_ASG.exe -net *> audit_build\missing_option.log
if ($LASTEXITCODE -eq 0) { throw 'Missing option value was accepted.' }
# removed options must be rejected
foreach ($removed in @('-resolving','-sttg_wsath')) {
    & .\audit_build\TDF_ASG.exe -net ./input/circuit/s27.v -fault ./input/rprf/s27_rpr_1.txt `
        -xor_tap ./input/xor_tap/tap_4.txt -scan_chain ./input/scan_chain/s27_sc_4.txt -power ON `
        -folder "./audit_removed_$suffix/" $removed ON *> audit_build\removed_option.log
    if ($LASTEXITCODE -eq 0) { throw "$removed was accepted." }
}
# the LFSR files are required only with the LFSR
& .\audit_build\TDF_ASG.exe -net ./input/circuit/s27.v -fault ./input/rprf/s27_rpr_1.txt -power ON `
    -folder "./audit_nolfsrfiles_$suffix/" *> audit_build\missing_lfsr_files.log
if ($LASTEXITCODE -eq 0) { throw '-xor_tap / -scan_chain were not required with the LFSR.' }

# .set file: CRLF line ends, experimental modes and options after -set
$setFile = "audit_build\crlf_$suffix.set"
$setLines = @('# CRLF setting file', '-net ./input/circuit/s27.v', '-fault ./input/rprf/s27_rpr_1.txt',
    '-xor_tap ./input/xor_tap/tap_4.txt', '-scan_chain ./input/scan_chain/s27_sc_4.txt',
    '-target_limit 2', '-time 5', '-power ON', '-fp ON')
[IO.File]::WriteAllText($setFile, ($setLines -join "`r`n") + "`r`n")
$setFolder = "./audit_set_$suffix/"
& .\audit_build\TDF_ASG.exe -set $setFile -folder $setFolder -wsath 25 *> audit_build\set_crlf.log
if ($LASTEXITCODE -ne 0) { throw '.set file with CRLF line ends failed.' }
$setLog = Get-Content "${setFolder}log.txt" -Raw
if ($setLog -notmatch 'FP mode \(-fp\)\s+:ON' -or $setLog -notmatch 'WSA threshold \(specified\)\s+:25%') { throw '.set options were not applied.' }
# a .set file without -power must be rejected, not crash
[IO.File]::WriteAllText($setFile, (($setLines | Where-Object { $_ -notmatch '^-power' }) -join "`n") + "`n")
& .\audit_build\TDF_ASG.exe -set $setFile -folder "./audit_set_nopower_$suffix/" *> audit_build\set_nopower.log
if ($LASTEXITCODE -eq 0) { throw '.set file without -power was accepted.' }
# an unknown option (typo) must be rejected, even with a complete .set file
[IO.File]::WriteAllText($setFile, ($setLines -join "`n") + "`n")
& .\audit_build\TDF_ASG.exe -set $setFile -folder "./audit_set_typo_$suffix/" -wsat 20 *> audit_build\set_unknown.log
if ($LASTEXITCODE -eq 0) { throw 'Unknown option was accepted.' }
