#!/bin/bash

cd $DEPLOYMENT_PATH/PANDORA/build/
make
echo "MAKE COMPLETED! ============================================================================================================"
sleep 1
make install
