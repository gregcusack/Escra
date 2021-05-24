#!/bin/bash

echo "MAKING AND INSERTING cgroup_connection...\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/cgroup_connection/KERN_SRC/cgroup_connection.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/cgroup_connection/KERN_SRC/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/cgroup_connection/KERN_SRC/cgroup_connection.ko

echo "MAKING AND INSERTING increase_mem_cgroup_margin...\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/increase_mem_cgroup_margin/increase_memcg_margin.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/increase_mem_cgroup_margin/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/increase_mem_cgroup_margin/increase_memcg_margin.ko

echo "MAKING AND INSERTING resize_max_mem...\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/resize_max_mem/resize_max_mem.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/resize_max_mem/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/resize_max_mem/resize_max_mem.ko

echo "MAKING AND INSERTING resize_quota..\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/resize_quota/KERN_SRC/resize_quota.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/resize_quota/KERN_SRC/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/resize_quota/KERN_SRC/resize_quota.ko

echo "MAKING AND INSERTING read_quota...\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_quota/KERN_SRC/read_quota.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_quota/KERN_SRC/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_quota/KERN_SRC/read_quota.ko

echo "MAKING AND INSERTING get_parent_cgid...\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/get_parent_cgid/KERN_SRC/get_parent_cgid.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/get_parent_cgid/KERN_SRC/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/get_parent_cgid/KERN_SRC/get_parent_cgid.ko

echo "MAKING AND INSERTING read_mem_usage...\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_mem_usage/read_mem_usage.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_mem_usage/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_mem_usage/read_mem_usage.ko

echo "MAKING AND INSERTING read_mem_limit...\n\n"
sudo rmmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_mem_limit/read_mem_limit.ko
cd /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_mem_limit/ && sudo make
sudo insmod /mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/read_mem_limit/read_mem_limit.ko

