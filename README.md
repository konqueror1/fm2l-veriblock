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
