#!/bin/bash

#####
## Script by Francisco Knebel
##
## Use scripts, you are not a computer.
## You are slow, let the machines work.
#####

# Define logging function
log()
{
  information=$1
  echo "$(date "+%Y-%m-%d %H:%M:%S"): $1"
}

log "Executing $0"
if [ -z "$1" ]
  then
    log "No machine name defined. Usage: './execute-experiments.sh <machine-name>'."
    log "Exiting..."
    exit 1
  else
    log "Machine name: $1"
fi

# Experiment variables (space separated values)
optimization_levels=("O0" "O1" "O2" "O3")
array_sizes=(1000 10000 100000 1000000 10000000 100000000)

# Cleans machine folder in experiments and creates experiment folders
for optimization in ${optimization_levels[@]}; do
  rm experiments/$1/$optimization -rf
  mkdir experiments/$1/$optimization -p

  for array_size in ${array_sizes[@]}; do
    mkdir experiments/$1/$optimization/$array_size -p
  done
done

# Execute experiment
for optimization in ${optimization_levels[@]}; do
  log "Compiling sort with optimization -$optimization"
  g++ -$optimization -fpermissive bench.c -o sort -w
  log "Done compiling."

  for array_size in ${array_sizes[@]}; do
    log "Executing $optimization/$array_size"
    ./rapl-tools/AppPowerMeter ./sort $array_size > experiments/$1/$optimization/$array_size/power.log
    cp rapl.csv experiments/$1/$optimization/$array_size/
    rm rapl.csv
  done
  rm sort
done

log "All experiments executed. Please check the 'experiments/$1' directory for the experiment results."
