#!/bin/bash

CONTAINER_NAME=$1
IP_ADDRESS=$2

num_subCont=5
CIDS=()
quota_array=( 200000 )
#quota_array=( 10000 20000 30000 40000 50000 60000 70000 80000 90000 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 )

export KERNEL_HOME="../"
make_dir="$(mkdir ec_subcontainers)"
echo "Made directory to store results"

cont_count=0
for i in "${quota_array[@]}"
do
	CIDS=()
	cont_count=0
	for j in $(seq 1 $num_subCont)
	do
		CONTAINER_ID="$(sudo docker run -d --cpu-period=100000 --cpu-quota="$i" --cpuset-cpus=0-9  "$CONTAINER_NAME")"
		CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
		echo "Running test for quota: "$i" with id: "$CONTAINER_ID" and pid: "$CONTAINER_PID""
		CIDS+=($CONTAINER_ID)
		# Here, we want to make this an "EC container"
		EC_RESULT="$(../ec_syscalls/sys_connect "$IP_ADDRESS" "$CONTAINER_PID" 4444)"
		echo "EC syscall result: "$EC_RESULT""
		echo "ran container: "$i" for quota: "$j""
	done
	echo "----------- Now waiting for containers to end running ----------"
	for y in "${CIDS[@]}"
	do
		cont_count=$((cont_count+1))
		while [ "`sudo docker inspect -f '{{.State.Running}}' "$y"`" != "false" ]
        	do
                	#echo "Container status: `sudo docker inspect -f '{{.State.Running}}' "$CONTAINER_ID"`"
                	sleep 1
        	done
		echo "container id "$y": exited"
		FILE_COPY="$(sudo docker cp "$y":/tmp/test.txt ec_subcontainers/"$i"_"$cont_count".txt)"
		echo "$(sudo docker rm "$y")"
		echo "Removing  container: "$y""
	done
done


