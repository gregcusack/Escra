#!/bin/bash

# while true; do sleep 0.004 ; echo "background print" >> /tmp/test.txt ; done
while true; do echo "background print" >> /tmp/test.txt ; done
#for i in seq {1..10}; do sysbench cpu --cpu-max-prime=100000 --events=100 --num-threads=100 run | grep "total time:" | awk '{print $3}' >> /tmp/test.txt; done

