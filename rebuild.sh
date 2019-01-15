#!/bin/sh
rm -rf build
mkdir build && cd build
cmake ../ || exit 1
make -j`nproc`