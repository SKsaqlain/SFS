#!/bin/bash
sudo apt-get install git
sudo apt-get install python3-pip
sudo -H pip3 install meson
sudo -H pip3 install ninja
sudo -H pip3 install pytest

cd ~/Desktop/
mkdir USP_Project
cd USP_Project/
git clone https://github.com/libfuse/libfuse.git
cd libfuse/

mkdir build
cd build
meson ..

cd ../
meson conf
meson conf -D disable-mtab=true

cd build
ninja
sudo python3 -m pytest test/
sudo ninja install

sudo chown root:root util/fusermount3
sudo chmod 4755 util/fusermount3
python3 -m pytest test/

sudo ln -s /usr/local/lib/x86_64-linux-gnu/libfuse3.so.3.2.1 /lib/x86_64-linux-gnu/libfuse3.so.3
