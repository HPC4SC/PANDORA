#!/bin/bash

cd ~/PANDORA/
rm -rf ~/PANDORA/build/ ~/PANDORA/install/
mkdir build/ install/

cd build/
cmake -D CMAKE_INSTALL_PREFIX=$PANDORAPATH ../
