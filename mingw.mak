#
# *** fuer Linux/GCC ***
#
#DEFINES = -Dgulp
DEFINES =
#DEFINES = -D__linux__ -D__MINGW__
CCOPT = -fexceptions -c -static
#-I/usr/X11R6/include -I/home/root/lesstif-0.75a/include
#STDLIBS =  -lm -lgcc -lstdc++ -lpthread
#STDLIBS =  -lm -lgcc -lstdc++
STDLIBS = -static-libstdc++ -static-libgcc -static 
#-L/usr/X11/lib -L/home/root/lesstif-0.75a/lib -lXm -lX11 -lXt -lICE -lSM -lXext
# -lncurses
EXEOPT =
DLLOPT = -fPIC -shared
INCLUDES = -I.
DORELEASE = -D_MIN_RELEASE -O2
# -O2
DODEBUG = -g -D_MIN_DEBUG
DOPROFILE = -g
DOPROFILEOBJ =
CC = g++
#CC = gcc
AR = ar
AROPT = r
AROPTPRE =
AROPTPOST =
AROPTFIN = s
OBJEXT = .o
LIBEXT = .a
DLLLIBEXT = .dll
EXEEXT = .exe
DLLEXT = .dll
OUTOPT = -o
EXEOUTOPT = -o
DLLOUTOPT = -o
INCOPT = -I

RC =
RCOPT =
RCEXT=
RESEXT=
HELPCOMP =
HELPEXT=
HELPOUTEXT=
DEL = rm
COPY = cp
MOVE = mv

OUTDIR = 
TARGETDIR = ../linux/
CDTOOUTDIR = cd ../linux
CDBACK = cd ../vis
DSTLIB = omsk_lg.a

selected = 1
with_pipe = 0
with_socket = 1

#!ifdef debug=1
#DEBUG = -g
#!else
DEBUG = 
#!endif

