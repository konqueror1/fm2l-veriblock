while true;
do
#    ./nodecore_pow_cuda -o pool.veriblockpool.com:8501 -u V8wB8DP56Cs9VrwyrXVsLTQ3e5Ngj5 -p FM2c;
  # ./veri_amd -o pool.veriblockpool.com:8501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun  -p FM2C ;
  #  ./veri_amd -o dabpool.com:8501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun  -p FM2C ;
  #  ./veri_amd -o vbk-fpga.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C ;
    ./veri_amd -o vbk.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C ;
   # ./veri_amd -o vbk.luckypool.io:9501 -u V8wB8DP56Cs9VrwyrXVsLTQ3e5Ngj5 -p FM2C -v true;
   # ./veri_amd -o vbk.luckypool.io:9501 -u V8wB8DP56Cs9VrwyrXVsLTQ3e5Ngj5 -p FM2C -v false;
   # ./veri_amd -o vbk-fpga.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C ;
#     ./veri_amd -o vbk-us.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C -l flase ;
   # ./veri_amd -o vbk.luckypool.io:9501 -u V8wB8DP56Cs9VrwyrXVsLTQ3e5Ngj5 -p FM2C ;
  echo "Restarting PoW miner!";
  sleep 1;
done;

