#!/bin/sh

export NDK_ROOT="/Users/min/Develop/android-ndk-r8b"
export PATH="$NDK_ROOT/toolchains/arm-linux-androideabi-4.4.3/prebuilt/darwin-x86/bin/:$PATH"
export SYS_ROOT="$NDK_ROOT/platforms/android-8/arch-arm/"
export CC="arm-linux-androideabi-g++ --sysroot=$SYS_ROOT"
export LD="arm-linux-androideabi-ld"
export AR="arm-linux-androideabi-ar"
export RANLIB="arm-linux-androideabi-ranlib"
export STRIP="arm-linux-androideabi-strip"
export MY_INCLUDES="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.4.3/include"
export MY_INCLUDES2="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.4.3/libs/armeabi/include"
export MY_LIBS="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.4.3/libs/armeabi"

make -f makefile.unx -e android=1 release=1

#echo $CC test.cpp -o testandro -I $MY_INCLUDES -I $MY_INCLUDES2 -L $MY_LIBS -lstdc++ -lgnustl_static

exit 0