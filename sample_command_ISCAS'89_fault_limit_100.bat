TDF_ASG.exe -net ./input/circuit/s5378.v -fault ./input/rprf/s5378_rpr_10_1.txt -seed seed.txt -xor_tap ./input/xor_tap/tap_32.txt -scan_chain ./input/scan_chain/s5378_sc_32.txt -power ON -target_limit 100 -time 60 -rand ON -folder ./s5378_output_100/

TDF_ASG.exe -net ./input/circuit/s9234.v -fault ./input/rprf/s9234_rpr_10_1.txt -seed seed.txt -xor_tap ./input/xor_tap/tap_32.txt -scan_chain ./input/scan_chain/s9234_sc_32.txt -power ON -target_limit 100 -time 60 -rand ON -folder ./s9234_output_100/

TDF_ASG.exe -net ./input/circuit/s13207.v -fault ./input/rprf/s13207_rpr_10_1.txt -seed seed.txt -xor_tap ./input/xor_tap/tap_64.txt -scan_chain ./input/scan_chain/s13207_sc_64.txt -power ON -target_limit 100 -time 60 -rand ON -folder ./s13207_output_100/

TDF_ASG.exe -net ./input/circuit/s15850.v -fault ./input/rprf/s15850_rpr_10_1.txt -seed seed.txt -xor_tap ./input/xor_tap/tap_64.txt -scan_chain ./input/scan_chain/s15850_sc_64.txt -power ON -target_limit 100 -time 60 -rand ON -folder ./s15850_output_100/

TDF_ASG.exe -net ./input/circuit/s38417.v -fault ./input/rprf/s38417_rpr_10_1.txt -seed seed.txt -xor_tap ./input/xor_tap/tap_64.txt -scan_chain ./input/scan_chain/s38417_sc_64.txt -power ON -target_limit 100 -time 60 -rand ON -folder ./s38417_output_100/

TDF_ASG.exe -net ./input/circuit/s38584.v -fault ./input/rprf/s38584_rpr_10_1.txt -seed seed.txt -xor_tap ./input/xor_tap/tap_64.txt -scan_chain ./input/scan_chain/s38584_sc_64.txt -power ON -target_limit 100 -time 60 -rand ON -folder ./s38584_output_100/

pause