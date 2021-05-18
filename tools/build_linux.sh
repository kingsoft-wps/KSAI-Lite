#!/bin/bash
set -e
set -x

cd ../tensorflow/lite/
case "$1" in
  native)
    mkdir -p ../../output/linux_sdk/native
    echo "target arch is native" 
    cmake -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_C_COMPILER=$(which gcc) ./ -B ../../output/linux_sdk/native
    cd ../../output/linux_sdk/native
    ;;
  cross_aarch64)
    mkdir -p ../../output/linux_sdk/cross_aarch64
    echo "target arch is aarch64" 
	# change toolchain to arm
    ARMCC_PREFIX=${HOME}/toolchains/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu/bin/aarch64-linux-gnu-
    ARMCC_FLAGS="-funsafe-math-optimizations"
    cmake ./ -B ../../output/linux_sdk/cross_aarch64\
      -DCMAKE_C_COMPILER=${ARMCC_PREFIX}gcc \
      -DCMAKE_CXX_COMPILER=${ARMCC_PREFIX}g++ \
      -DCMAKE_C_FLAGS="${ARMCC_FLAGS}" \
      -DCMAKE_CXX_FLAGS="${ARMCC_FLAGS}" \
      -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
      -DCMAKE_SYSTEM_NAME=Linux \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64
    cd ../../output/linux_sdk/cross_aarch64
    ;;
  cross_mips64)
    mkdir -p ../../output/linux_sdk/cross_mips64
    echo "target arch is mips64" 
    # change toolchain to mips64, for example
    MIPS64CC_PREFIX=mips64el-linux-gnuabi64-
    cmake ./ -B ../../output/linux_sdk/cross_mips64 \
    -DCMAKE_C_COMPILER=${MIPS64CC_PREFIX}gcc \
    -DCMAKE_CXX_COMPILER=${MIPS64CC_PREFIX}g++ \
    -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=mips64el
    cd ../../output/linux_sdk/cross_mips64
	  ;;
  *)
    echo "please input the target arch you want to build" 
    ;;
esac
#make -j4
make
make benchmark_model
make reg_test
mkdir -p ./lib/
mv libtensorflow-lite* ./lib/
cd ../../../tools

