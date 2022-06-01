#!/bin/bash
set -e
set -x

cd ../tensorflow/lite
mkdir -p ../../output/android_sdk/

cmake ./ -B ../../output/android_sdk \
  -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a 
cd ../../output/android_sdk/
cmake --build .
make benchmark_model
make reg_test