# FM2L VBK miner source code patched.

compile : `make`

In Windows version you can change the COM3 or COM2 , like this way.

### when build in windows , please refer original build documentation

Use MSVS project `.sln` please, and in setting you can modify library path on your Visual Studio installation.

If you build this in Windows version share us. or PR.

**Bounty Jobs for 5000TEO**
**And, You will be a chance with work with our team**

PR and Issue , for this repository with your TEO address.


# Thanks,

# updates 2019/06/14
Windows10 WSL (Windows Subsystem Linux) based VBK mining

refer Wiki ::
https://github.com/trustcoinmining/fm2l-veriblock/wiki/WSL-(Windows-Subsystem-for-Linux)%EB%A5%BC-%EC%9D%B4%EC%9A%A9%ED%95%9C-Windows10-%EC%97%90%EC%84%9C-vbk-miner-%EC%9A%B4%EC%98%81%ED%95%98%EA%B8%B0.


# updates 2019/06/13

If you want to flashing permanantly bitstream, download on releases section *.mcs.zip and *.prm 
and flashing it.

[refer here](https://github.com/trustcoinmining/TCM-FMx/blob/master/VIVADO-MCS-File-Flashing.docx)
it takes long time for 5~10minutes.

And, also you need to connect Xilinx jtag driver , please contact our technical supports.
please care a JTAG Line direction carefully. it is shared our telegram and discord.


### adds ubuntu multi gui miner start scripts.

**rungui.sh** and **runveri.sh** has added for easy customization
after download above file

```
chmod +x rungui.sh runveri.sh

```
and customize your pool, wallet , workername , usbdevice name in **rungui.sh**

```
# edit rungui.sh 
#  ./runveri.sh  {pool-url} {Your wallet} {workername} {usb device name}
```

** also 
```
# you can check your device is connected by dmesg
#   dmesg
#  
```
**cp210x** is FM2L usb device.
**ttyUSB0** is usb connected tty name
**if you connect multiple FM2L card , ttyUSB? is increased**

```
dmesg

.....

[524071.711459] cp210x 1-11:1.0: **cp210x** converter detected
[524071.713105] usb 1-11: cp210x converter now attached to ttyUSB0
[526407.725114] usb 1-11: USB disconnect, device number 12
# [526407.725378] cp210x ttyUSB0: cp210x converter now disconnected from ttyUSB0
# [526407.725430] cp210x 1-11:1.0: device disconnected

```

edit **rungui.sh** 

```
gedit rungui.sh
```

and run

```
./rungui.sh

```
**rungui.sh** examples.

```
#!/bin/sh

gnome-terminal -e " ./runveri.sh vbk.luckypool.io:9501 V6hhHVMxdcRzfT2rNFr1kmVq86riun FM2w1 /dev/ttyUSB0 "
gnome-terminal -e " ./runveri.sh vbk.luckypool.io:9501 V6hhHVMxdcRzfT2rNFr1kmVq86riun FM2w2 /dev/ttyUSB1 "
gnome-terminal -e " ./runveri.sh vbk.luckypool.io:9501 V6hhHVMxdcRzfT2rNFr1kmVq86riun FM2w3 /dev/ttyUSB2 "
gnome-terminal -e " ./runveri.sh vbk.luckypool.io:9501 V6hhHVMxdcRzfT2rNFr1kmVq86riun FM2w4 /dev/ttyUSB3 "
```

it is all.

----------


## Command Line Arguments
<a name="command_line"></a>
```VeriBlock vBlake FM2L FPGA Miner v4.12
Required Arguments:
-o <poolAddress>           The pool address to mine to in the format host:port
-u <username>              The username (often an address) used at the pool

-nl <devicelists>          The multiple FPGA port lists, case sensitive

Optional Arguments:
-p <password>              The miner/worker password to use on the pool
-d <deviceNum>             The ordinal of the device to use (default 0)
-l <enableLogging>         Whether to log to a file (default true)
-v <enableVerboseOutput>   Whether to enable verbose output for debugging (default false)
```

Example command line:
```
sudo veri_fm2l -o vbk.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C  -nl /dev/ttyUSB0,/dev/ttyUSB1

veri_fm2l.exe -o vbk.luckypool.io:9501 -u V6hhHVMxdcRzfT2rNFr1kmVq86riun -p FM2C  -nl COM2,COM8,COM9
```


<br><br>
<br><br>
## Compiling on Linux (Ubuntu 16.04)

`make` is enough
