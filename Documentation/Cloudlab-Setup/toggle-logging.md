## Remove kernel logging for DC ##
1) go into: `/mnt/ECKernel/Distributed-Containers/EC-4.20.16/ec_modules/debug-defs.h`
change `#define DEBUG_LOGS 1` to `#define DEBUG_LOGS 0`
2) make clean, make, and insert all of the kernel modules

## Remove GCM logging: ##
1) `sudo rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake`
2) `cmake -DCMAKE_BUILD_TYPE=Release -DWITH_DEBUG=info -DCMAKE_C_COMPILER=/usr/bin/gcc-8 -DCMAKE_CXX_COMPILER=/usr/bin/g++-8 .`
3) `make -j40`

## Compiling on unmodified kernel 4.20.16 ##
`cd /mnt/ECKernel/Distributed-Containers/EC-4.20.15`
`git checkout stable`
`cp -v /boot/config-$(uname -r) .config`
`sudo make menuconfig`
`sudo make -j40 && sudo make -j40 modules_install && sudo make -j40 install`
`sudo reboot`