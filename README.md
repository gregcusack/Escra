# Deploying Distributed Containers on CloudLab

Disclaimer: Note that the following instructions are for deploying Distributed Containers with the integration of k8s on a CloudLab cluster (with the ElasticContainerTestbed Profile). This readme is meant to be as comprehensive as possible and so if any steps are missing and/or incorrect, please update it accordingly. 

## CloudLab Cluster Initialization
The CloudLab cluster is setup in the following manner: One GCM master node and three worker nodes where all the worker nodes have the kernel image 4.20.16EC+. 

### Instructions to Setup CloudLab Cluster
1. Create a CloudLab Experiment with the ElasticContainerTestbed Profile
### Master Node
2. On the GCM Master Node, run the following commands to setup the environment:
  * #### Map volume to specific directory
    1. `sudo mkfs.ext4 /dev/sda4`
    2. `sudo mkdir /mnt/ECKernel`
    3. `sudo mount /dev/sda4 /mnt/ECKernel`
    4. `sudo chown -R <username>:root /mnt/ECKernel`
  * #### Clone GCM repo and checkout appropriate branch
    1. `git clone https://github.com/gregcusack/ec_gcm.git`
    2. `git checkout ftr_k8s_integration_cont_creation `
  * #### Install Required Libraries
    The GCM requires a couple of libraries/dependencies to work correctly. Most notably - an updated cmake package, and the c++restsdk. 
    
    ##### 1. Install cmake via the following commands. Note that instructions for this were pulled from this [source](https://askubuntu.com/questions/355565/how-do-i-install-the-latest-version-of-cmake-from-the-command-line)
    ``` 
    version=3.16
    build=2
    mkdir ~/temp
    cd ~/temp
    wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
    tar -xzvf cmake-$version.$build.tar.gz
    cd cmake-$version.$build/ 
    sudo ./bootstrap
    sudo make -j$(nproc)
    sudo make install
    ```
    ##### 2. Install [CPPRestSDK](https://github.com/microsoft/cpprestsdk). Instructions were pulled from [here](https://github.com/microsoft/cpprestsdk/wiki/How-to-build-for-Linux) 
    ```
    sudo apt-get update
    sudo apt-get install g++ git libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev libwebsocketpp-dev openssl libssl-dev ninja-build
    ```
    ```
    git clone https://github.com/Microsoft/cpprestsdk.git casablanca
    ```
    At the next point, you can change the build type to Release, instead of Debug
    ```
    cd casablanca
    mkdir build.debug
    cd build.debug
    cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Debug
    ninja
    ```
    To install on the system, run:
    ```
    sudo ninja install
    ```

    ##### 3. Install the [Google Protobuf](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md) Compiler. i.e installation commands are as follows but for more detailed installation instructions, use the github link
    ```
    sudo apt-get install autoconf automake libtool curl make g++ unzip
    git clone https://github.com/protocolbuffers/protobuf.git
    cd protobuf
    git submodule update --init --recursive
    ./autogen.sh
    ./configure
     sudo make
     sudo make check
     sudo make install
     sudo ldconfig # refresh shared library cache.
    ```
  
    ##### 4. Install Ansible to deploy Agents on the different hosts. More detailed instructions can be found [here](https://docs.ansible.com/ansible/latest/installation_guide/intro_installation.html#installing-ansible-on-ubuntu)
    ```
    sudo apt update
    sudo apt install software-properties-common
    sudo apt-add-repository --yes --update ppa:ansible/ansible
    sudo apt install ansible
    ```
    ##### 5. Install YAML-CPP via:
      * `sudo apt-get update`
      * `sudo apt-get install libyaml-cpp-dev`
      
    ##### 6. Install [Docker](https://docs.docker.com/install/linux/docker-ce/ubuntu/)
    ##### 7. Install Kubernetes: A more detailed instructional guide can be found [here](https://blog.sourcerer.io/a-kubernetes-quick-start-for-people-who-know-just-enough-about-docker-to-get-by-71c5933b4633)
      * `sudo apt-get update && sudo apt-get install -y apt-transport-https`
      * `sudo curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -`
      * Create/edit the file: /etc/apt/sources.list.d/kubernetes.list to add `deb http://apt.kubernetes.io/ kubernetes-xenial main`
      * `sudo apt-get update`
      * `sudo apt-get install -y kubelet kubeadm kubectl`
    


  * #### Build project using cmake
    Finally, assure that the project builds successfully using cmake:
    ```
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ .
    ```
    Note that you might have to update the path/version of where g++ is installed on your machine. 

### Worker Nodes
3. On each of the worker nodes, run the following commands to setup the environment
  * #### Map volume to specific directory
    1. `sudo mkfs.ext4 /dev/sda4`
    2. `sudo mkdir /mnt/ECKernel`
    3. `sudo mount /dev/sda4 /mnt/ECKernel`
    4. `sudo chown -R <username>:root /mnt/ECKernel`
  * #### Clone EC-4.20.16 repo, build, and compile the kernel
    1. `git clone https://github.com/gregcusack/EC-4.20.16.git`
    2. `git checkout ftr-k8s-integration`
    3. `cp -v /boot/config-$(uname -r) .config`
    4. `sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev`
    5. `make menuconfig`
    6. `sudo make -j$(nproc) && sudo make -j$(nproc) modules_install && sudo make -j$(nproc) install`
    7. `sudo reboot`

    Note: Whenever you reboot a machine in Cloudlab, you'll have to remount the /mnt/ dir. You can accomplish this via: `sudo mount /dev/sda4 /mnt/ECKernel`
  * #### Install Required Libraries
  1. The worker nodes require an updated cmake package to build the Agent. Instructions are the same as the GCM master node to build and install cmake:
  ```
    version=3.16
    build=2
    mkdir ~/temp
    cd ~/temp
    wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
    tar -xzvf cmake-$version.$build.tar.gz
    cd cmake-$version.$build/ 
    sudo ./bootstrap
    sudo make -j$(nproc)
    sudo make install
  ``` 
  2. Install [Docker](https://docs.docker.com/install/linux/docker-ce/ubuntu/)
  3. Install Kubernetes: A more detailed instructional guide can be found [here](https://blog.sourcerer.io/a-kubernetes-quick-start-for-people-who-know-just-enough-about-docker-to-get-by-71c5933b4633)
      * `sudo apt-get update && sudo apt-get install -y apt-transport-https`
      * `curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -`
      * Edit the file: /etc/apt/sources.list.d/kubernetes.list to add `deb http://apt.kubernetes.io/ kubernetes-xenial main`
      * `sudo apt-get update`
      * `sudo apt-get install -y kubelet kubeadm kubectl`
  4. Install latest version of Go (version 1.14.1 has been tested):
      * `sudo apt-get update `
      * `cd /tmp`
      * `wget https://dl.google.com/go/go1.14.1.linux-amd64.tar.gz`
      * `sudo tar -xvf go1.14.1.linux-amd64.tar.gz`
      * `sudo mv go /usr/local`
      * Add the Following lines in ~/.profile
        * `export GOROOT=/usr/local/go`
        * `export GOPATH=$HOME/go`    
        * `export PATH=$GOPATH/bin:$GOROOT/bin:$PATH`
      * `source ~/.profile`


## K8s Cluster Initialization
Finally, we are at the point where we can create a kubernetes cluster. We can accomplish this via the following commands:

  ### Master Node
  1. Initialize Kubeadm

    export MASTER_IP=<master_ip>
    sudo kubeadm init --apiserver-advertise-address $MASTER_IP
    mkdir -p $HOME/.kube
    sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
    sudo chown $(id -u):$(id -g) $HOME/.kube/config

    sudo cp /etc/kubernetes/admin.conf $HOME/
    sudo chown $(id -u):$(id -g) $HOME/admin.conf
    export KUBECONFIG=$HOME/admin.conf

    sudo sysctl net.bridge.bridge-nf-call-iptables=1
    export kubever=$(kubectl version | base64 | tr -d '\n')
    kubectl apply -f "https://cloud.weave.works/k8s/net?k8s-version=$kubever"

    
  Note that if you see an error related to swap after the initial kubeadm init, the following command might help:
    `sudo swapoff -a && sudo sed -i '/swap/d' /etc/fstab` 
  and rerun the kubeadm init command in the earlier codeblock. 

  2. Upon a successful control plane initialization, the output for the command to join the cluster should be something like:
  ```
  Your Kubernetes control-plane has initialized successfully!
  ...
  kubeadm join 192.168.6.8:6443 --token 8f849s.j7fdp3v7s3p2fb9s..
  ```

  #### 2(a). Note that if you want a one node cluster setup (i.e to test on localhost), you have to untaint the master node to be able to run pods. In order to do this, you don't have to use the kubeadm join command and instead issue the following command on the master node:
  `kubectl taint nodes --all node-role.kubernetes.io/master-`  
    
  ### Worker Nodes
  1. To join the cluster that the master node is advertising, use the `kubeadm join ...` command that the master node spits out as part of the last step


Now when you check the cluster status from the master node, you should see the worked nodes as well. i.e.
```
PreritO@gcm:/mnt/ECKernel/ec_gcm$ kubectl get nodes
NAME                                                  STATUS   ROLES    AGE     VERSION
gcm.ectest.cudevopsfall2018-pg0.wisc.cloudlab.us      Ready    master   4m15s   v1.17.2
node-1.ectest.cudevopsfall2018-pg0.wisc.cloudlab.us   Ready    <none>   61s     v1.17.2
```

And this marks a successful k8s cluster deployment!

## Deploying a Distributed Container

Finally, we're at the step where we can deploy a distributed container application. To do so, we have to take the following steps on the master and worker nodes:

### Worker Nodes
1. Start the Agent processes on each of the worker nodes to run in the background
2. Start the cAdvisor process on each of the worker nodes by navigating to directory: EC-4.20.16/ec_gcm/cAdvisorSDK/cadvisor and entering the following commands:
    * `make build`
    * `sudo ./cadvisor`

### Master Node
1. Run a kube proxy via the command: `kubectl proxy -p 8000` 
2. Build and run the ec_gcm application with the specified application definition file (in JSON format) using the following commands: `./ec_gcm tests/app_def.json`


<!-- ### Master Nodes
1. Start the Agent processes on the worker nodes. We accomplish this via Ansible. However, before ansible can successfully start the Agent process, we need to add the ssh-key for the worker nodes onto the GCM master node, as outlined [here](http://www.linuxproblem.org/art_9.html)
2. Once the ssh-keys for the worker nodes have been added to the Master GCM node, we can then run the ansible playbook to deploy the agent process on all the hosts: 
`ansible-playbook -b -v -u <user> execute-agent.yaml -kkkk --extra-vars "host-group" -i host-group` where host-group is a file containing the hostnames for the worker nodes. 
3. Finally, create an application definition file JSON file. An example file is in the ec_gcm git repos under the `tests/` directory and titled: "app_def.json". Run the ec_gcm program with this json file as an argument: i.e. `./ec_gcm tests/app_def.json` -->
