#
# *** fuer Zaurus Linux/GCC ***
#
#DEFINES = -Dgulp
CC	=	arm-linux-gcc
CXX	=	arm-linux-g++
MYCC=	arm-linux-g++
DEFINES = -D __ZAURUS__
CCOPT = -fexceptions -c
#-I/usr/X11R6/include -I/home/root/lesstif-0.75a/include
STDLIBS =  -lm -lgcc -lstdc++ -ldl
#-L/usr/X11/lib -L/home/root/lesstif-0.75a/lib -lXm -lX11 -lXt -lICE -lSM -lXext
# -lncurses
EXEOPT = -s
DLLOPT = -fPIC -shared -s
INCLUDES = -I.
#DORELEASE = -O2 -D_MIN_RELEASE
DORELEASE = -D_MIN_RELEASE
DODEBUG = -g -D_MIN_DEBUG
DOPROFILE = -g
DOPROFILEOBJ =
CC = gcc
AR = ar
AROPT = r
AROPTPRE =
AROPTPOST =
AROPTFIN = s
OBJEXT = .o
LIBEXT = .a
DLLLIBEXT = .so.1.0
EXEEXT =
DLLEXT = .so.1.0
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

