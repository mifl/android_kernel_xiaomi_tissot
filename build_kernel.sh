#!/bin/bash

#
# Kernel Build Script
#

# Set variables
BUILD_KERNEL_OUT_DIR=KERNEL_OUT
BUILD_JOB_NUMBER=`grep processor /proc/cpuinfo | wc -l`
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

# Set defconfig
DEFCONFIG_FILE=$2
if [ ! -e arch/$BUILD_SUBARCH/configs/$DEFCONFIG_FILE ]; then
	echo "No such file : arch/$BUILD_SUBARCH/configs/$DEFCONFIG_FILE"
	exit -1
fi

# Build kernel
mkdir -p ./$BUILD_KERNEL_OUT_DIR/
make O=./$BUILD_KERNEL_OUT_DIR/ ARCH=$BUILD_SUBARCH ${DEFCONFIG_FILE}
make O=./$BUILD_KERNEL_OUT_DIR/ ARCH=$BUILD_SUBARCH -j$BUILD_JOB_NUMBER 2>&1 | tee $BUILD_KERNEL_LOG
