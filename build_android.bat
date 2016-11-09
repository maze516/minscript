rem call ..\visiscript\set_android_env.bat

set NDK_ROOT=C:\usr\android-ndk-r11c
set PATH=%NDK_ROOT%\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin;%PATH%
set SYS_ROOT=%NDK_ROOT%\platforms\android-9\arch-arm
set CC=arm-linux-androideabi-g++ --sysroot=C:\usr\android-ndk-r11c\platforms\android-9\arch-arm
set LD=arm-linux-androideabi-ld
set AR=arm-linux-androideabi-ar
set RANLIB=arm-linux-androideabi-ranlib
set STRIP=arm-linux-androideabi-strip
set MY_INCLUDES=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\include
set MY_INCLUDES2=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\armeabi\include
set MY_LIBS=%NDK_ROOT%\sources\cxx-stl\gnu-libstdc++\4.9\libs\armeabi
set CFLAGS=-fPIC --sysroot=%SYS_ROOT%
set LDFLAGS=-pie

del *.o
mingw32-make -j 4 -f makefile.unx -e android_win=1 release=1
copy minscript minscript_O2_android
C:\usr\android-ndk-r11c\toolchains\arm-linux-androideabi-4.9\prebuilt\windows-x86_64\bin\arm-linux-androideabi-strip minscript_O2_android
rem del *.o
rem mingw32-make -f makefile.unx -e android=1 debug=1
rem copy minscript minscript_d_android

rem exit 0