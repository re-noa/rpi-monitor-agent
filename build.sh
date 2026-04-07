#!/bin/bash

set -e

BUILD_DIR="build"

if [ "$1" == "clean" ]; then
    rm -rf $BUILD_DIR
    echo "[Info] Dossier build supprime."
fi

mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake ..
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "-----------------------------------"
    echo "[Success] Compilation terminee."
    echo "[Run] ./build/monitor_agent"
    echo "-----------------------------------"
fi