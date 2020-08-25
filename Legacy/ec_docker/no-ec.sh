#!/bin/bash

CONTAINER_NAME=$1

CONTAINER_ID="$(sudo docker run -d --cpu-period=100000 --cpu-quota=100000 --cpuset-cpus 0 "$CONTAINER_NAME")"
CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
echo "container id: "$CONTAINER_ID""
echo "container init pid: "$CONTAINER_PID""
