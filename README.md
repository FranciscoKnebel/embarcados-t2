# Quadsort

Version:  1.1.3.2

## Compile
```
g++ -O0 -fpermissive bench.c -o sort
```
Ignore warnings.

## Execute benchmark
```
./sort
```
Source: https://github.com/scandum/quadsort

Array size is the first parameter. Defaults to 10000.

## RAPL Tools

Monitoring CPU power.

Download rapl-tools, from https://github.com/kentcz/rapl-tools.
Can also be found in `rapl-tools` directory.

### Requirements

* GNU Make. G++ >= 4.7

### Setup

Run make to compile.
Before using the tools, you will need to load the msr module and adjust the permissions.

```
# Install and load the msr module
sudo apt-get install msr-tools
sudo modprobe msr

# Set permissions
sudo chmod o+rw /dev/cpu/0/msr
```
Recent versions of the linux kernel require special permission to access the msr.
```
sudo apt-get install libcap2-bin
sudo setcap cap_sys_rawio+ep ./AppPowerMeter
sudo setcap cap_sys_rawio+ep ./PowerMonitor
```

## Experiment Setup

We will experiment with quadsort compiled multiple times, varying optimization levels.
The used values are: -O0 (default), -O1, -O2 and -O3.

https://www.rapidtables.com/code/linux/gcc/gcc-o.html

After building quadsort and rapl-tools, execute the following commands:
```
./rapl-tools/AppPowerMeter sort 1000 > experiments/<machine-name>/<optimization-level>/1000.log
./rapl-tools/AppPowerMeter sort 10000 > experiments/<machine-name>/<optimization-level>/10000.log
./rapl-tools/AppPowerMeter sort 100000 > experiments/<machine-name>/<optimization-level>/100000.log
./rapl-tools/AppPowerMeter sort 1000000 > experiments/<machine-name>/<optimization-level>/1000000.log
```

Save the log files in a specific folder for the current experiment.
Repeat for each optimization.
