#!/bin/bash

#
# Kernel Clean Script
#

# Set variables
BUILD_KERNEL_OUT_DIR=KERNEL_OUT
BUILD_KERNEL_LOG=kernel_log.txt

# Set Sub arch
BUILD_SUBARCH=$1

# Set toolchain
export ARCH=$BUILD_SUBARCH
if [ "$BUILD_SUBARCH" = "arm" ]; then
	GCC_SUBARCH=arm
	GCC_SUBDIR=arm-eabi
	GCC_VERSION=4.8
elif [ "$BUILD_SUBARCH" = "arm64" ]; then
	GCC_SUBARCH=aarch64
	GCC_SUBDIR=aarch64-linux-android
	GCC_VERSION=4.9
fi
export PATH=$(pwd)/../../../prebuilts/gcc/linux-x86/${GCC_SUBARCH}/${GCC_SUBDIR}-${GCC_VERSION}/bin:$PATH
export CROSS_COMPILE=${GCC_SUBDIR}-

# Clean compiled files
make mrproper
make O=./$BUILD_KERNEL_OUT_DIR/ clean

# Delete out dir
if [ -d ./$BUILD_KERNEL_OUT_DIR/ ]
then
	rm -r ./$BUILD_KERNEL_OUT_DIR/
fi

# Delete build log
if [ -f ./$BUILD_KERNEL_LOG ]
then
	rm ./$BUILD_KERNEL_LOG
fi
