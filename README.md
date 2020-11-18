# Quadsort

Version:  1.1.3.2

## Requirements
* g++

`sudo apt install build-essential`

## Compile
```
g++ -O0 -fpermissive bench.c -o sort
```
Ignore warnings.

## Execute quadsort benchmark
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
Check the Compile section for details on how to compile quadsort and execute the experiments for each level.
The used values are: -O0 (default), -O1, -O2 and -O3.

https://www.rapidtables.com/code/linux/gcc/gcc-o.html

Array size values are: 1000, 10000, 100000, 1000000, 10000000 and 100000000.

### Manual experiments

After building quadsort and rapl-tools, execute the following commands, for each array size:
```
./rapl-tools/AppPowerMeter ./sort <array-size> > experiments/<machine-name>/<optimization-level>/<array-size>/power.log
cp rapl.csv experiments/<machine-name>/<optimization-level>/<array-size>/
```

This will execute AppPowerMeter with the compiled `sort` binary and save the results to the specific experiment folder. Save the log files in a specific folder for the each experiment.
Repeat for each optimization.

### Automated experiments

Script that executes all variations.
Compile rapl-tools as described above and execute the script:
```
./execute-experiments.sh <machine-name>
```

The argument `<machine-name>` will be a directory in `experiments`, containing the directories for the optimizations used and the array sizes.
