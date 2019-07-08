while true;
do
#    
  # ./veri_fm2l -o pool.veriblockpool.com:8501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun  -p FM2C ;
  #  ./veri_fm2l -o dabpool.com:8501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun  -p FM2C ;
  #  ./veri_fm2l -o vbk-fpga.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C ;
    ./veri_fm2l -o vbk.thedreadpool.eu:8501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C -nl /dev/ttyS8 -l 1 ;
   # ./veri_fm2l -o vbk.luckypool.io:9501 -u V8wB8DP56Cs9VrwyrXVsLTQ3e5Ngj5 -p FM2C -v true;
   # ./veri_fm2l -o vbk.luckypool.io:9501 -u V8wB8DP56Cs9VrwyrXVsLTQ3e5Ngj5 -p FM2C -v false;
   # ./veri_fm2l -o vbk-fpga.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C ;
#     ./veri_fm2l -o vbk-us.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C -l flase ;
   # ./veri_fm2l -o vbk.luckypool.io:9501 -u V8wB8DP56Cs9VrwyrXVsLTQ3e5Ngj5 -p FM2C ;
  echo "Restarting veri_FM2L miner!";
  sleep 1;
done;

