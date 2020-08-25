#!/bin/bash
dyn_pid=$1
bash_pid=$2
samples=$3
period=$4
filename="/home/greg/Desktop/plots/${dyn_pid}_${bash_pid}_cpu_util.csv"

top -bn $samples -d $period | grep -E -w "$dyn_pid|$bash_pid" | awk '{print $9}' > "$filename"
