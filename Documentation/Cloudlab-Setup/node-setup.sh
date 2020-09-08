#!/bin/bash

uname=$1

uname=gcusack
sudo mkfs.ext4 /dev/sda4
sudo mkdir /mnt/ECKernel
sudo mount /dev/sda4 /mnt/ECKernel
sudo chown -R $uname:root /mnt/ECKernel

sudo swapoff -a

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

# INSTALL: go v1.14.4
curl -O https://storage.googleapis.com/golang/go1.14.4.linux-amd64.tar.gz
tar -xvf go1.14.4.linux-amd64.tar.gz
sudo mv go /usr/local

echo 'export GOPATH=$HOME/go' | sudo tee -a ~/.profile
echo 'export PATH=$PATH:/usr/local/go/bin:$GOPATH/bin' | sudo tee -a ~/.profile
source ~/.profile
go version
cd ~

# INSTALL: grpc go
export GO111MODULE=on
go get github.com/golang/protobuf/protoc-gen-go
export PATH="$PATH:$(go env GOPATH)/bin"
cd ~

#INSTALL EC-Agent
git clone git@github.com:gregcusack/Distributed-Containers.git /mnt/ECKernel/Distributed-Containers
cd /mnt/ECKernel/Distributed-Containers
git submodule update --init -- EC-Agent/
cd EC-Agent
git checkout ftr-delete-pod
cd ..
git submodule update --init -- third_party/cadvisor/
cd third_party/cadvisor
make build
cd ../..
git submodule update --init -- third_party/DeathStarBench/
cd third_party/DeathStarBench
git checkout k8s-support
cd ../../


# INSTALL: kernel and compile and reboot
cd /mnt/ECKernel/Distributed-Containers
git submodule update --init -- EC-4.20.16/
cd EC-4.20.16
git checkout bug-mem-ONLY

cp -v /boot/config-$(uname -r) .config
sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev
sudo make menuconfig
sudo make -j20 && sudo make -j20 modules_install && sudo make -j20 install
sudo reboot

# Run the following on reboot
# sudo swapoff -a
# sudo mount /dev/sda4 /mnt/ECKernel
# sudo kubeadm join 128.105.144.171:6443 --token <token-from-gcm-output> \
#    --discovery-token-ca-cert-hash <sha-val-from-gcm-output>