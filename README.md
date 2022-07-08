# Deploying Distributed Containers on CloudLab

Disclaimer: Note that the following instructions are for deploying Distributed Containers with the integration of k8s on a CloudLab cluster. This readme is meant to be as comprehensive as possible and so if any steps are missing and/or incorrect, please let us know!

# Cloudlab
This work has been deployed and tested in cloudlab using the following Cloudlab Github repository. Please see [THIS](https://github.com/hunhoffe/ec-cloudlab) repo for deploying Escra in cloudlab. The cloudlab profile is configrable for a few different hardware types and a variable number of compute nodes. The Escra Controller ([ec_gcm](https://github.com/gregcusack/ec_gcm)) and Deployer ([ec_deployer](https://github.com/gregcusack/ec_deployer)) are deployed on a control node. The Escra Agents ([EC-Agent](https://github.com/Maziyar-Na/EC-Agent)) and Escra Linux Kernel ([EC-4.20.16](https://github.com/gregcusack/EC-4.20.16)) are deployed on the worker nodes. 

Once all nodes are deployed, they should be connected to each other. Run `kubectl get nodes` on the `gcm` node to verify. 
Next, setup ssh-keys for each node and add them to github. 
Example. for each node run:
```
ssh-keygen
# hit enter a bunch of times
cat ~/.ssh/id_rsa.pub
```
copy output of above command into github

Once your ssh-keys are added to github. You can continue following [THESE](https://github.com/hunhoffe/ec-cloudlab) instructions. 
1) Run `/local/repository/gcm_setup.sh` on the control node (gcm node)
2) Run `/local/repository/node_setup/sh` on the worker nodes


Note to start periodic reclamation after all the containers are deployed, you need to set the value that is stored in Manager.cpp in ec_gcm

# Example: media-microsvc:

On each agent run the following:
```
cd /mydata/ec/Escra/third_party/cadvisor && sudo ./cadvisor &
cd /mydata/ec/Escra/EC-Agent && go run main.go
```

On the control node, run:
```
cd /mydata/ec/Escra/ec_gcm 
./ec_gcm test/app_def.json
```

and in another terminal window on the control node run:
```
kubectl create ns media-microsvc

cd /mydata/ec/Escra/ec_deployer
go run main.go -f app_deploy.json

```

If you see this error:
```
[DBG] Successfully opened app_deploy.json
[DBG] Configuring K8s ClientSet
panic: stat /users/<cloudlab-username>/.kube/config: no such file or directory

goroutine 1 [running]:
main.configK8(0x1709960)
	/mydata/ec/Escra/ec_deployer/main.go:260 +0x239
main.main()
	/mydata/ec/Escra/ec_deployer/main.go:65 +0x391
exit status 2
```

Run the following which will print out the path to kubectl
```
kubectl get pods -v=6
```

Run 
```
ln -s <output-from-command-above> /users/<cloudlab-username>/.kube/config
```
example:
```
ln -s /home/ec/.kube/config /users/<cloudlab-username>/.kube/config
```


