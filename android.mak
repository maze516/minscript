#
# *** fuer Android/GCC ***
#
#DEFINES = -Dgulp
DEFINES =
MYCC = $$CC
CCOPT = -fexceptions -c 
#-fPIC
#-I/usr/X11R6/include -I/home/root/lesstif-0.75a/include
#STDLIBS =  -lm -lgcc -lg++ -lpthread -ldl
#STDLIBS =  -lm -lgcc -lg++ -ldl
STDLIBS =  -L $$MY_LIBS -lm -lstdc++ -lgnustl_static 
#-L/usr/X11/lib -L/home/root/lesstif-0.75a/lib -lXm -lX11 -lXt -lICE -lSM -lXext
# -lncurses
EXEOPT =
DLLOPT = -fPIC -shared
INCLUDES = -I $$MY_INCLUDES -I $$MY_INCLUDES2 -I.
DORELEASE = -O2 -D_MIN_RELEASE
DODEBUG = -g -D_MIN_DEBUG
DOPROFILE = -g
DOPROFILEOBJ = 
CC = g++
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

