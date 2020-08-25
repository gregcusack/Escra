#!/bin/bash

CONTAINER_NAME=$1
IP=$2
PORT=$3
QUOTA=$(($4 * 1000))
VOL=$5
echo $QUOTA

CONTAINER_ID="$(sudo docker run -d --cpu-period=100000 --cpu-quota=$QUOTA --cpuset-cpus 0 --mount source=$VOL,target=/app "$CONTAINER_NAME")"

CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
echo "container id: "$CONTAINER_ID""
echo "container init pid: "$CONTAINER_PID""

export KERNEL_HOME="../"
../ec_syscalls/sys_connect "$IP" "$CONTAINER_PID" "$PORT"
export CONTAINER_ID
