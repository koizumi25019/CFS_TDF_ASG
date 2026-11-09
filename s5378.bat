TDF_ASG.exe -net ./input/circuit/s5378.v -fault ./input/rprf/s5378_rpr_1.txt -xor_tap ./input/xor_tap/tap_32.txt -scan_chain ./input/scan_chain/s5378_sc_32.txt -power ON -target_limit 300 -time 60 -folder ./s5378_output/ -parallel 24
pause
TDF_ASG.exe -set ./input/script/s5378.set