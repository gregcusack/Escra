#!/bin/bash

uname=$1

sudo mkfs.ext4 /dev/sda4
sudo mkdir /mnt/ECKernel
sudo mount /dev/sda4 /mnt/ECKernel
sudo chown -R $uname:root /mnt/ECKernel

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

# INSTALL: cmake
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

# INSTALL: casablanca
git clone git@github.com:microsoft/cpprestsdk.git casablanca

cd casablanca
mkdir build.debug
cd build.debug
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Debug
ninja

sudo ninja install
cd ~

# INSTALL: Docker
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

# INSTALL: Kubernetes
sudo apt-get update && sudo apt-get install -y apt-transport-https
sudo curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg | sudo apt-key add -
echo 'deb http://apt.kubernetes.io/ kubernetes-xenial main' | sudo tee -a /etc/apt/sources.list.d/kubernetes.list

sudo apt-get update
sudo apt-get install -y kubelet kubeadm kubectl


# INSTALL gcc-8
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install -y gcc-8 g++-8

# INSTALL: protobuf
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


# INSTALL: grpc --> C++
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

# INSTALL: go v1.14.4
curl -O https://storage.googleapis.com/golang/go1.14.4.linux-amd64.tar.gz
tar -xvf go1.14.4.linux-amd64.tar.gz
sudo mv go /usr/local

echo 'export GOPATH=$HOME/go' | sudo tee -a ~/.profile
echo 'export PATH=$PATH:/usr/local/go/bin:$GOPATH/bin' | sudo tee -a ~/.profile
source ~/.profile
go version
cd ~

# Install: grpc go
export GO111MODULE=on
go get github.com/golang/protobuf/protoc-gen-go
export PATH="$PATH:$(go env GOPATH)/bin"

# install SPDLOG - logging for GCM
cd ~/Distributed-Containers/third_party/spdlog
mkdir build && cd build
cmake .. && make -j && sudo make install
cd ~

#Setup Kubernetes
sudo swapoff -a
export MASTER_IP=$(hostname -i)
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

#After worker nodes run: kubeadm join..., run kubectl get nodes



# RUN MEM TEST
# kubectl exec -it -n media-microsvc <pod-name> -- /bin/bash
# apt update && apt install stress
# stress -m 1 --vm-bytes <bytes-to-alloc>m --timeout 10s --verbose

#Deathstar bench for kubernetes
# git checkout 4e50355d7058175c87d628a121798ad110f424b3

#INSTALL: Python 3
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

# COMPILE GCM
cd ~/Distributed-Containers/ec_gcm 
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/gcc-8 -DCMAKE_CXX_COMPILER=/usr/bin/g++-8 .
make -j20


# SETUP: Deathstar Bench
# Change lines 55, 59, 63, and 67 in <path-to-repo>/DeathStarBench/mediaMicroservices/k8s-yaml/nginx-web-server.yaml which refers to the the installation directory location of DeathStarBench to the appropriate location
# Use kubectl -n media-microsvc get svc nginx-web-server to get its cluster-ip.
# Paste the cluster ip at <path-of-repo>/mediaMicroservices/scripts/write_movie_info.py:99 and <path-of-repo>/mediaMicroservices/scripts/register_users.sh:5 
# 3.Update <path-of-repo>/mediaMicroservices/scripts/write_movie_info.py:95 & 97 to point to the installation path of the repo
# python3 <path-of-repo>/mediaMicroservices/scripts/write_movie_info.py && <path-of-repo>/mediaMicroservices/scripts/register_users.sh
# And finally, to run the an instance of the HTTP workload generator (i.e. compose reviews for the movies)
# Paste the cluster ip at <path-of-repo>/mediaMicroservices/wrk2/scripts/media-microservices/compose-review.lua:1032
# cd <path-of-repo>/mediaMicroservices/wrk2
# ./wrk -D exp -t <num-threads> -c <num-conns> -d <duration> -L -s ./scripts/media-microservices/compose-review.lua http://<cluster-ip>:8080/wrk2-api/review/compose -R <reqs-per-sec>



