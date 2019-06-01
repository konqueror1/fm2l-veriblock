@echo off
SET APP_TITLE=VeriBlock FPGA FM2L Miner
TITLE %APP_TITLE%

:Execute

 veri_fm2l -o vbk.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C -nl COM8,COM2,COM3
 
echo ----------------------------------------------------------------------
echo ^| FPGA FM2L Miner is restarting in 1 seconds!                             ^|
echo ^| Press CTRL+C to cancel this or ENTER to proceed immediately...     ^|
echo ----------------------------------------------------------------------
timeout /t 1 > NUL
echo Restarting %%APP_TITLE%%...
goto Execute
