# Running DSB workload
1. Get IP address of service:
```
SVC_IPADDR=$(kubectl -n media-microsvc get svc nginx-web-server -o jsonpath='{.spec.clusterIP}')
```

2. Run Register movies/users step
```
cd ~/Distributed-Containers/third_party/DeathStarBench/mediaMicroservices/scripts
python3.7 write_movie_info.py --server_ip $SVC_IPADDR:8080 -c ../datasets/tmdb/casts.json -m ../datasets/tmdb/movies.json && ./register_users.sh $SVC_IPADDR
```

3. Run workload. This is configurable:
```
cd ~/Distributed-Containers/third_party/DeathStarBench/mediaMicroservices/scripts
../wrk2/wrk -D exp -t <num-threads> -c 1<num-conns> -d <duration>s -L -s ../wrk2/scripts/media-microservices/compose-review.lua http://$SVC_IPADDR:8080/wrk2-api/review/compose -R <reqs-per-sec>
```

### Note: 
You shouldn't have to change lines 55, 59, 63, and 67 in `<path-to-repo>/DeathStarBench/mediaMicroservices/k8s-yaml/nginx-web-server.yaml` to the the installation directory location of DeathStarBench if you followed the `insert-mods` steps. DSB should be in `/mnt/ECKernel/Distributed-Containers/third_party/DeathStarBench/`

**ALSO:** This is the path to the repo of DSB on the WORKER NODES. But you're entering the value on the GCM. This tells worker node where to get nginx info from on the workern node

-----------------------------

#### LEGACY NOTES. SHOULDN'T HAVE TO WORRY ABOUT STUFF BELOW. BUT CAN USE TO TROUBLESHOOT IF ISSUES
Running DeathStar Bench
1. Register users and movie information
	1. Use ```kubectl -n media-microsvc get svc nginx-web-server``` to get its cluster-ip

2. Paste the cluster ip at ```<path-of-repo>/mediaMicroservices/scripts/write_movie_info.py:99``` and ```<path-of-repo>/mediaMicroservices/scripts/register_users.sh:5```

3. Update ```<path-of-repo>/mediaMicroservices/scripts/write_movie_info.py:95 & 97``` to point to the installation path of the repo

4. Run: ```python3 <path-of-repo>/mediaMicroservices/scripts/write_movie_info.py && <path-of-repo>/mediaMicroservices/scripts/register_users.sh```

5. And finally, to run the an instance of the HTTP workload generator (i.e. compose reviews for the movies)
	1. Paste the cluster ip at ```<path-of-repo>/mediaMicroservices/wrk2/scripts/media-microservices/compose-review.lua:1032```

6. ```cd <path-of-repo>/mediaMicroservices/wrk2```

```./wrk -D exp -t <num-threads> -c <num-conns> -d <duration> -L -s ./scripts/media-microservices/compose-review.lua http://<cluster-ip>:8080/wrk2-api/review/compose -R <reqs-per-sec>```


