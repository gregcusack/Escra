#!/bin/bash

CONTAINER_NAME=$1

CONTAINER_ID="$(sudo docker run -d "$CONTAINER_NAME")"
CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
echo "container id: "$CONTAINER_ID""
echo "container init pid: "$CONTAINER_PID""

export KERNEL_HOME="../"
../ec_syscalls/sys_connect "$CONTAINER_PID"
