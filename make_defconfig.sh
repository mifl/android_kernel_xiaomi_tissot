#!/bin/bash

BUILD_SUBARCH=$1
if [ "$BUILD_SUBARCH" = "arm" ]; then
	GCC_SUBDIR=arm-eabi
elif [ "$BUILD_SUBARCH" = "arm64" ]; then
	GCC_SUBDIR=aarch64-linux-android
fi

DEFCONFIG_FILE=$2
if [ -z "$DEFCONFIG_FILE" ]; then
	echo "Need defconfig file(msm_defconfig)!"
	exit -1
fi

if [ ! -e arch/$BUILD_SUBARCH/configs/$DEFCONFIG_FILE ]; then
	echo "No such file : arch/$BUILD_SUBARCH/configs/$DEFCONFIG_FILE"
	exit -1
fi

# make .config
env KCONFIG_NOTIMESTAMP=true \
make ARCH=$BUILD_SUBARCH CROSS_COMPILE=$GCC_SUBDIR- ${DEFCONFIG_FILE}

# run menuconfig
env KCONFIG_NOTIMESTAMP=true \
make menuconfig ARCH=$BUILD_SUBARCH
make savedefconfig ARCH=$BUILD_SUBARCH

# copy .config to defconfig
mv defconfig arch/$BUILD_SUBARCH/configs/${DEFCONFIG_FILE}

# clean kernel object
make mrproper
