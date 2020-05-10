cmake_minimum_required (VERSION 3.1)

project (minscript)

set (CMAKE_CXX_STANDARD 11)

add_executable(minscript minscript.cpp
    minip.cpp
    minipenv.cpp
    minparser.cpp
    minparsernodes.cpp
    minpreproc.cpp
    mintokenizer.cpp
    osdep.cpp
    minip.h
    minipenv.h
    minparser.h
    minparsernodes.h
    minpreproc.h
    mintokenizer.h
    minnativehlp.h
    minhandle.h
    platform.h
    osdep.h)

set_target_properties(minscript PROPERTIES LINK_FLAGS_RELEASE -s)

install(TARGETS minscript DESTINATION bin)

if (CMAKE_SYSTEM_NAME MATCHES "Android")
    add_custom_command(TARGET minscript POST_BUILD
            COMMAND "${ANDROID_TOOLCHAIN_PREFIX}strip" -g -S -d --strip-debug --verbose
            minscript
            COMMENT "Strip debug symbols done on final minscript binary.")
endif()
            