# Steps for Setting up Cloud Controller on Cloudlab #

### MOUNT ECKERNEL 
```
uname=<username>
sudo mkfs.ext4 /dev/sda4
sudo mkdir /mnt/ECKernel
sudo mount /dev/sda4 /mnt/ECKernel
sudo chown -R $uname:root /mnt/ECKernel
```

### DOWNLOAD GIT REPO
```
git clone git@github.com:gregcusack/Distributed-Containers.git
cd Distributed-Containers
git submodule update --init -- ec_gcm/
cd ec_gcm
git checkout ftr-delete-pod
cd ..
git submodule update --init -- ec_deployer/
cd ec_deployer
git checkout bug-mem-ONLY
cd ..
git submodule update --init -- third_party/DeathStarBench/
cd third_party/DeathStarBench
git checkout k8s-support
cd ..
git submodule update --init -- third_party/spdlog/
cd ~
```

### INSTALL: cmake
```
version=3.16
build=2
mkdir ~/temp
cd ~/temp
wget https://cmake.org/files/v$version/cmake-$version.$build.tar.gz
tar -xzvf cmake-$version.$build.tar.gz
cd cmake-$version.$build/ 
sudo ./bootstrap
sudo make -j10
sudo make install

sudo apt-get update
sudo apt-get install -y g++ git libboost-atomic-dev libboost-thread-dev libboost-system-dev libboost-date-time-dev libboost-regex-dev libboost-filesystem-dev libboost-random-dev libboost-chrono-dev libboost-serialization-dev libwebsocketpp-dev openssl libssl-dev ninja-build
cd ~
```

### INSTALL: casablanca
```
git clone git@github.com:microsoft/cpprestsdk.git casablanca
cd casablanca
mkdir build.debug
cd build.debug
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Debug
ninja
sudo ninja install
cd ~
```

### INSTALL: Docker
```
sudo apt-get update
sudo apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    gnupg-agent \
    software-properties-common

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo apt-key fingerprint 0EBFCD88

sudo add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
   $(lsb_release -cs) \
   stable"

sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io
```

### INSTALL: Kubernetes
```
sudo apt-get update && sudo apt-get install -y apt-transport-https
sudo curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
echo 'deb http://apt.kubernetes.io/ kubernetes-xenial main' | sudo tee -a /etc/apt/sources.list.d/kubernetes.list

sudo apt-get update
sudo apt-get install -y kubelet kubeadm kubectl
```

### INSTALL gcc-8
```
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install -y gcc-8 g++-8
```

### INSTALL: protobuf
```
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
git submodule update --init --recursive
./autogen.sh
 ./configure
make -j12
make -j12 check
sudo make -j12 install
sudo ldconfig
cd ~
```

### INSTALL: grpc --> C++
```
sudo apt-get install -y build-essential autoconf libtool pkg-config

git clone --recurse-submodules -b v1.28.1 https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
cd cmake/build

cmake ../.. -DgRPC_INSTALL=ON                \
              -DCMAKE_BUILD_TYPE=Release       \
              -DgRPC_PROTOBUF_PROVIDER=package \
              -DgRPC_SSL_PROVIDER=package \
              -DBUILD_SHARED_LIBS=ON \
              -DCMAKE_INSTALL_PREFIX=/usr/local

make -j20
sudo make install
cd ~
```

### INSTALL: go v1.14.4
```
curl -O https://storage.googleapis.com/golang/go1.14.4.linux-amd64.tar.gz
tar -xvf go1.14.4.linux-amd64.tar.gz
sudo mv go /usr/local

echo 'export GOPATH=$HOME/go' | sudo tee -a ~/.profile
echo 'export PATH=$PATH:/usr/local/go/bin:$GOPATH/bin' | sudo tee -a ~/.profile
source ~/.profile
go version
cd ~
```

### Install: grpc go
```
export GO111MODULE=on
go get github.com/golang/protobuf/protoc-gen-go
export PATH="$PATH:$(go env GOPATH)/bin"
```

### Install SPDLOG - logging for GCM
```
cd ~/Distributed-Containers/third_party/spdlog
mkdir build && cd build
cmake .. && make -j && sudo make install
cd ~
```

### Setup Kubernetes
* Set K8s to run on the private cloudlab IP
* In file: `/etc/systemd/system/kubelet.service.d/10-kubeadm.conf`
* Change line `Environment="KUBELET_CONFIG_ARGS=--config=/var/lib/kubelet/config.yaml"` to:
`Environment="KUBELET_CONFIG_ARGS=--config=/var/lib/kubelet/config.yaml --node-ip=<private-net-ip>"`
* Run: `sudo systemctl daemon-reload` followed by: `sudo systemctl restart kubelet`

```
sudo swapoff -a
```

* Two options: 
1) If you want to run on public IP, run: `export MASTER_IP=$(hostname -i)`
2) If you want to run on private network IP, get IP via `hostname- I` and then run: `export MASTER_IP=<private-net-ip>`

```
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
```

* After worker nodes run: kubeadm join..., run kubectl get nodes

### INSTALL: Python 3
```
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt update -y
sudo apt install -y python3.7

sudo apt install -y python3-pip
python3.7 -m pip install --upgrade pip
python3.7 -m pip install asyncio
python3.7 -m pip install aiohttp

sudo apt install -y luarocks
sudo luarocks install luasocket
sudo apt install -y libmemcached-dev
```

### COMPILE GCM
```
cd ~/Distributed-Containers/ec_gcm 
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc-8 -DCMAKE_CXX_COMPILER=/usr/bin/g++-8 .
make -j20
```

### RUN MEM TEST ###
```
kubectl exec -it -n media-microsvc <pod-name> -- /bin/bash
apt update && apt install stress
stress -m 1 --vm-bytes <bytes-to-alloc>m --timeout 10s --verbose
```
