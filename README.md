# Deploying Distributed Containers on CloudLab

Disclaimer: Note that the following instructions are for deploying Distributed Containers with the integration of k8s on a CloudLab cluster. This readme is meant to be as comprehensive as possible and so if any steps are missing and/or incorrect, please let us know!

# Cloudlab
This work has been deployed and tested in cloudlab using the following Cloudlab Github repository. Please see [THIS](https://github.com/hunhoffe/ec-cloudlab) repo for deploying Escra in cloudlab. The cloudlab profile is configrable for a few different hardware types and a variable number of compute nodes. The Escra Controller ([ec_gcm](https://github.com/gregcusack/ec_gcm)) and Deployer ([ec_deployer](https://github.com/gregcusack/ec_deployer)) are deployed on a control node. The Escra Agents ([EC-Agent](https://github.com/Maziyar-Na/EC-Agent)) and Escra Linux Kernel ([EC-4.20.16]([git@github.com:gregcusack/EC-4.20.16.git](https://github.com/gregcusack/EC-4.20.16))) are deployed on the worker nodes. 

Once all nodes are deployed, they should be connected to each other. Run `kubectl get nodes` on the `gcm` node to verify. 
Next, setup ssh-keys for each node and add them to github. 
Once your ssh-keys are added to github. You can continue following [THESE](https://github.com/hunhoffe/ec-cloudlab) instructions. 
1) Run `/local/repository/gcm_setup.sh` on the control node (gcm node)
2) Run `/local/repository/node_setup/sh` on the worker nodes