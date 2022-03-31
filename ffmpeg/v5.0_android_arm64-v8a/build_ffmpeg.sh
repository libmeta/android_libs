#!/bin/bash
#你自己的NDK路径.
export NDK=/opt/android-sdk/ndk/21.3.6528147 
#armv8-a
ARCH=arm64
CPU=armv8-a
API=21
TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64
SYSROOT=$NDK/platforms/android-$API/arch-$ARCH/
ISYSROOT=$NDK/sysroot
ASM=$ISYSROOT/usr/include
CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android$API-
PREFIX=../ffmpeg_libs/
OPTIMIZE_CFLAGS="-march=$CPU"
CC=${CROSS_PREFIX}clang
CXX=${CROSS_PREFIX}clang++
ANDROID_CROSS_PREFIX=$TOOLCHAIN/bin/aarch64-linux-android-
STRIP=${ANDROID_CROSS_PREFIX}strip
NM=${ANDROID_CROSS_PREFIX}nm
AR=${ANDROID_CROSS_PREFIX}ar
RANLIB=${ANDROID_CROSS_PREFIX}ranlib
INCLUDE_PATH="-I../ffmpeg_libs/include -I$ASM -I$ASM/aarch64-linux-android -I$ASM/aarch64-linux-android/asm"
LIBS_PATH="-L$SYSROOT/usr/lib -L../ffmpeg_libs/lib"
PKG_CONFIG=$(which pkg-config)
#export PKG_CONFIG_PATH=../ffmpeg_libs/lib/pkgconfig:$PKG_CONFIG_PATH
OPENSSL_PKG_CONFIG=/home/x/work/libs/ffmpeg/srt-1.4.4/scripts/build-android/openssl-3.0.2
SRT_PKG_CONFIG=/home/x/work/libs/ffmpeg/srt-1.4.4/scripts/build-android/arm64-v8a/lib/pkgconfig
export PKG_CONFIG_PATH=$SRT_PKG_CONFIG:$OPENSSL_PKG_CONFIG:$PKG_CONFIG_PATH

./configure \
    --prefix=$PREFIX \
    --enable-postproc \
    --enable-shared \
    --disable-static \
    --enable-libsrt \
    --enable-openssl \
    --disable-debug \
    --disable-doc \
    --enable-ffmpeg \
    --disable-ffplay \
    --disable-ffprobe \
    --disable-doc \
    --disable-symver \
    --disable-vulkan \
    --cross-prefix=$CROSS_PREFIX \
    --target-os=android \
    --arch=$ARCH \
    --cpu=$CPU \
    --cc=$CC \
    --cxx=$CXX \
    --strip=$STRIP \
    --nm=$NM \
    --ar=$AR \
    --ranlib=$RANLIB \
    --sysroot=$SYSROOT \
    --pkg-config=$PKG_CONFIG \
    --enable-cross-compile \
    --extra-cflags="-isysroot=$ISYSROOT -Os -fpic -D__ANDROID_API__=$API ${INCLUDE_PATH}" \
    --extra-ldflags="${LIBS_PATH} " 
    
#OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU "

