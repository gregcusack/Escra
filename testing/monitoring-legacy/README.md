# Instructions to run Prometheus on K8s cluster

## Install metrics server 
1. Install metrics server via: 
    ```
    kubectl apply -f https://github.com/kubernetes-sigs/metrics-server/releases/download/v0.3.6/components.yaml
    ```
2. Verify that the metrics-server deployment is running the desired number of pods with the following command.
    ```
    kubectl get deployment metrics-server -n kube-system
    ```

    Ouput should look like: 
    ```
    NAME             READY   UP-TO-DATE   AVAILABLE   AGE
    metrics-server   1/1     1            1           6m
    ```

3. At this point, run the following command to check if node/pod statistics are available `kubectl top notes`. If you see the following error, then continue to step 4, else, skip to Promethus setup section
    ```
    error: metrics not available yet
    ```

4. In order to fix the previous error, you need to edit the metrics-server deployment to add a few arguments. You can do so via the following steps:
    - `kubectl edit deploy metrics-server -n kube-system`
    - Edit that file to add the flags: `--kubelet-insecure-tls` and `--kubelet-preferred-address-types=InternalIP` in spec.template.spec.containers.arg
    i.e.
    ```
    ...
    spec:
      containers:
      - args:
        - --cert-dir=/tmp
        - --secure-port=4443
        - --kubelet-insecure-tls
        - --kubelet-preferred-address-types=InternalIP
    ...
    ```
    - Upon changing the file, metrics-server deployment should redeploy automatically and after waiting ~2 mins, if you run `kubectl top nodes`, you should see node usage. i.e.
    ```
    NAME                                                         CPU(cores)   CPU%   MEMORY(bytes)   MEMORY%
    gcm.ectestbedc220.cudevopsfall2018-pg0.wisc.cloudlab.us      320m         0%     4645Mi          2%
    node-1.ectestbedc220.cudevopsfall2018-pg0.wisc.cloudlab.us   75m          0%     1232Mi          0%
    node-2.ectestbedc220.cudevopsfall2018-pg0.wisc.cloudlab.us   66m          0%     1227Mi          0%
    node-3.ectestbedc220.cudevopsfall2018-pg0.wisc.cloudlab.us   76m          0%     1223Mi          0%
    ```

    [source](https://docs.aws.amazon.com/eks/latest/userguide/metrics-server.html)


## Run Promethus
1. To setup prometheus, create monitoring namespace:
    ```
    kubectl create ns monitoring
    ```
2. Run the `build.sh` script in this directory. 
3. Verify all resources are running in the monitoring namespace via: `kubectl get all -n monitoring`. If everything is working correctly, you should see all pods running similar to:
    ```
    NAME                                      READY   STATUS    RESTARTS   AGE
    pod/kube-state-metrics-758cdbb755-52lrz   1/1     Running   0          2m10s
    pod/prometheus-core-9db84bbc7-mnt7d       1/1     Running   0          2m11s
    pod/prometheus-node-exporter-6hq8j        1/1     Running   0          2m16s
    pod/prometheus-node-exporter-kwh6z        1/1     Running   0          2m16s
    pod/prometheus-node-exporter-xp8zb        1/1     Running   0          2m16s

    NAME                               TYPE        CLUSTER-IP       EXTERNAL-IP   PORT(S)          AGE
    service/kube-state-metrics         ClusterIP   10.107.116.161   <none>        8080/TCP         2m11s
    service/prometheus                 NodePort    10.96.11.6       <none>        9090:30871/TCP   2m16s
    service/prometheus-node-exporter   ClusterIP   None             <none>        9100/TCP         2m16s

    NAME                                      DESIRED   CURRENT   READY   UP-TO-DATE   AVAILABLE   NODE SELECTOR   AGE
    daemonset.apps/prometheus-node-exporter   3         3         3       3            3           <none>          2m16s

    NAME                                 READY   UP-TO-DATE   AVAILABLE   AGE
    deployment.apps/kube-state-metrics   1/1     1            1           2m11s
    deployment.apps/prometheus-core      1/1     1            1           2m11s

    NAME                                            DESIRED   CURRENT   READY   AGE
    replicaset.apps/kube-state-metrics-758cdbb755   1         1         1       2m11s
    replicaset.apps/prometheus-core-9db84bbc7       1         1         1       2m11s
    ```
4. To view prometheus on local machine, navigate to the the following web address: `http://<GCM-node-public-ip>:<prometheus-port>`
    Note that the promethus port is printed out in the previous output in format: 

    ```
    service/prometheus                 NodePort    10.96.11.6       <none>        9090:30871/TCP   2m16s
    ```

5. Run CPU/Memory queries on prometheus. For instance, for CPU, we have:
    ```
    sum(rate(container_cpu_usage_seconds_total{pod!="~jaeger*", image!="", container_name!="POD", namespace="media-microsvc"}[1m])) by (pod) 
    ```
    And for memory:
    ```
    sum(container_memory_usage_bytes{pod!="jaeger*", image!="", container_name!="POD", namespace="media-microsvc"}) by (pod)
    ```

6. To quickly shut down all components, you can just delete that namespace: `kubectl delete ns monitoring`

Source: I heavily referenced the [this repo](https://github.com/giantswarm/prometheus/blob/master/manifests-all.yaml) to set up this folder but modified it for our needs