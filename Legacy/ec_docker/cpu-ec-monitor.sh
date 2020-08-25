#!/bin/bash
dyn_pid=$1
bash_pid=$2
filename="/home/greg/Desktop/plots/${dyn_pid}_${bash_pid}_cpu_util.csv"

top -bn 200 -d .5 | grep -E -w "$dyn_pid|$bash_pid" | awk '{print $9}' > "$filename"
