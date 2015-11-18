#!/bin/sh

export NDK_ROOT="/Users/min/Develop/android-ndk-r10d"
export PATH="$NDK_ROOT/toolchains/x86-4.9/prebuilt/darwin-x86_64/bin/:$PATH"
export SYS_ROOT="$NDK_ROOT/platforms/android-9/arch-x86/"
export CC="i686-linux-android-gcc --sysroot=$SYS_ROOT"
export LD="i686-linux-android-ld"
export AR="i686-linux-android-ar"
export RANLIB="i686-linux-android-ranlib"
export STRIP="i686-linux-android-strip"
export MY_INCLUDES="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.9/include"
export MY_INCLUDES2="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.9/libs/x86/include"
export MY_LIBS="$NDK_ROOT/sources/cxx-stl/gnu-libstdc++/4.9/libs/x86"

make -f makefile.unx -e android=1 release=1
cp minscript minscript_O2_android_x86
i686-linux-android-strip minscript_O2_android
rm *.o
make -f makefile.unx -e android=1 debug=1
cp minscript minscript_d_android_x86

exit 0