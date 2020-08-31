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
git clone git@github.com:Maziyar-Na/EC-Agent.git
cd EC-Agent
git checkout ftr-delete-pod
cd ~

# INSTALL: gcm for cadvisor stuff
git clone --recurse-submodules git@github.com:gregcusack/ec_gcm.git
cd ec_gcm
git checkout ftr-delete-pod
git submodule update --init --recursive
cd cAdvisorSDK/cadvisor
make build
cd ~


# INSTALL: kernel and compile and reboot
git clone git@github.com:gregcusack/EC-4.20.16.git /mnt/ECKernel/EC-4.20.16
cd /mnt/ECKernel/EC-4.20.16
git checkout ftr-bug-ONLY

cp -v /boot/config-$(uname -r) .config
sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev
sudo make menuconfig
sudo make -j20 && sudo make -j20 modules_install && sudo make -j20 install
sudo reboot

# Run the following on reboot
# sudo mount /dev/sda4 /mnt/ECKernel
# sudo kubeadm join 128.105.144.171:6443 --token <token-from-gcm-output> \
#    --discovery-token-ca-cert-hash <sha-val-from-gcm-output>