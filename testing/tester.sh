#!/bin/bash

num_tests=$1
rate=$2
duration=$3
ratio=$4
test_type=$5 #DC or K8s

skipsetup=$6 # 0 don't skip. 1 skip

SVC_IPADDR=$(kubectl -n media-microsvc get svc nginx-web-server -o jsonpath='{.spec.clusterIP}')

if [ "$skipsetup" -eq "0" ]; then
	sleep 1

	python3.7 /users/gcusack/Distributed-Containers/third_party/DeathStarBench/mediaMicroservices/scripts/write_movie_info.py --server_ip $SVC_IPADDR:8080 -c /users/gcusack/Distributed-Containers/third_party/DeathStarBench/mediaMicroservices/datasets/tmdb/casts.json -m /users/gcusack/Distributed-Containers/third_party/DeathStarBench/mediaMicroservices/datasets/tmdb/movies.json

	sleep 1
fi

for ((i=0;i<$num_tests;i++)); 
do 

	filename="${test_type}-${ratio}-${i}.txt"
	echo "RUN: ${i}, filename: ${filename}"
	/users/gcusack/Distributed-Containers/third_party/DeathStarBench/mediaMicroservices/wrk2/wrk -P -t 1 -c 10 -d $duration -s /users/gcusack/Distributed-Containers/third_party/DeathStarBench/mediaMicroservices/wrk2/scripts/media-microservices/compose-review.lua http://$SVC_IPADDR:8080/wrk2-api/review/compose -R $rate

	sleep 1	

	cp /users/gcusack/0.txt /mnt/ECKernel/test-results/"${test_type}-${ratio}-${i}.txt" 

done



