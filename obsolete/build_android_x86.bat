rem call ..\visiscript\set_android_env.bat

set NDK_ROOT=C:\usr\android-ndk-r15c
set PATH=%NDK_ROOT%\toolchains\x86-4.9\prebuilt\windows-x86_64\bin;%PATH%
set SYS_ROOT=%NDK_ROOT%\platforms\android-9\arch-arm
set CC=i686-linux-android-g++ --sysroot=C:\usr\android-ndk-r15c\platforms\android-9\arch-x86
set LD=i686-linux-android-ld
set AR=i686-linux-android-ar
set RANLIB=i686-linux-android-ranlib
set STRIP=i686-linux-android-strip
set MY_INCLUDES=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\include
#set MY_INCLUDES2=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\x86\include
set MY_LIBS=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\x86
set CFLAGS=-fPIC --sysroot=%SYS_ROOT%
set LDFLAGS=-pie

del *.o
mingw32-make -j 4 -f makefile.unx -e android_win=1 release=1
copy minscript minscript_O2_android_x86
C:\usr\android-ndk-r15c\toolchains\x86-4.9\prebuilt\windows-x86_64\bin\i686-linux-android-strip minscript_O2_android_x86
rem del *.o
rem mingw32-make -f makefile.unx -e android=1 debug=1
rem copy minscript minscript_d_android

rem exit 0