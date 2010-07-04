#! /bin/bash

export RUN_DIR=$PWD
cd ../../..
source test_env.sh
cd $RUN_DIR

python ../../../../clients/simple/client.py ./config
