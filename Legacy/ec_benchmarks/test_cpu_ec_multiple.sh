#!/bin/bash

CONTAINER_NAME=$1
IP_ADDRESS=$2

# This scrript assumes just one subcontainer within each "distributed container"
ec_cont_ports=( 4444 4445 )
CIDS=()
quota_array=( 200000 )
#quota_array=( 10000 20000 30000 40000 50000 60000 70000 80000 90000 100000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 )

export KERNEL_HOME="../"
make_dir="$(mkdir ec_multiple)"
echo "Made directory to store results"

cont_count=0
for i in "${quota_array[@]}"
do
	CIDS=()
	cont_count=0
	for j in "${ec_cont_ports[@]}"
	do
		CONTAINER_ID="$(sudo docker run -d --cpu-period=100000 --cpu-quota="$i" --cpuset-cpus=0-9  "$CONTAINER_NAME")"
		CONTAINER_PID="$(sudo docker inspect -f '{{.State.Pid}}' "$CONTAINER_ID")"
		echo "Running test for quota: "$i" with id: "$CONTAINER_ID" and pid: "$CONTAINER_PID""
		CIDS+=($CONTAINER_ID)
		# Here, we want to make this a seperate "distributed container"
		EC_RESULT="$(../ec_syscalls/sys_connect "$IP_ADDRESS" "$CONTAINER_PID" "$j")"
		echo "EC syscall result: "$EC_RESULT""
		echo "Started container with quota="$i" on port "$j"..."
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
		FILE_COPY="$(sudo docker cp "$y":/tmp/test.txt ec_multiple/"$i"_"$cont_count".txt)"
		echo "$(sudo docker rm "$y")"
		echo "Removing  container: "$y""
	done
done


