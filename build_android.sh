#!/bin/sh

export NDK_ROOT="/Users/min/Develop/android-ndk-r9c"
export PATH="$NDK_ROOT/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64/bin/:$PATH"
export SYS_ROOT="$NDK_ROOT/platforms/android-9/arch-arm/"
export CC="arm-linux-androideabi-g++ --sysroot=$SYS_ROOT"
export LD="arm-linux-androideabi-ld"
export AR="arm-linux-androideabi-ar"
export RANLIB="arm-linux-androideabi-ranlib"
export STRIP="arm-linux-androideabi-strip"
export MY_INCLUDES="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.8/include"
export MY_INCLUDES2="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/include"
export MY_LIBS="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi"

make -f makefile.unx -e android=1 release=1
cp minscript minscript_O2_android
rm *.o
make -f makefile.unx -e android=1 debug=1
cp minscript minscript_d_android

exit 0