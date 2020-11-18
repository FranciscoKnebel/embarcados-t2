# Quadsort

Version:  1.1.3.2

## Compile

g++ -O3 -fpermissive bench.c -o sort

## Execute benchmark

./sort

Source: https://github.com/scandum/quadsort


## RAPL Tools

Monitoring CPU power.

Download rapl-tools, from https://github.com/kentcz/rapl-tools.
Can also be found in `rapl-tools` directory.

### Requirements

* GNU Make. G++ >= 4.7

### Setup

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
