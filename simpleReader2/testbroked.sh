#!/bin/bash

sudo ../../3d-re/bin/3D-reconstruction
sudo chmod 777 test.oni
ln -s test.oni in.oni
./build/imgTest

