#
# *** fuer MSC 5.x ***
#
#DEFINES = /Dgulp
DEFINES =
WARNINGS =
CCOPT = /c /MD /GX /GR $(WARNINGS)
CCDLLOPT = 
CCEXEOPT =
DORELEASE = /O2 /G5 /D_UM_RELEASE
DODEBUG = /Zi /Fm /D_UM_DEBUG
DOPROFILE = /Zi
DOPROFILEOBJ = # /P
EXEOPT = /MD 
LINKOPT = /link /FIXED:NO
# /link[/FIXED:NO]
DLLOPT = /MD /LD /GD
STDLIBS = user32.lib gdi32.lib comdlg32.lib
INCLUDES =  
 # /I../uvbase/defs /I../grit
GRITDIR = e:\grit41
IMPLIB =
CDEXE = cd
CC = cl
AR = lib
ARFIRST = /OUT:
AROPT = 
AROPTPRE = 
AROPTPOST = 
AROPTFIN = 
OBJEXT = .obj
LIBEXT = .lib
DLLLIBEXT = .lib
EXEEXT = .exe
DLLEXT = .dll
OUTOPT = /Fo
EXEOUTOPT = /Fe
DLLOUTOPT = /Fe
INCOPT = /I

MONICORE = monicorewnt

RC = rc
RCOPT = -r -DWIN32
RCEXT=.rc
RESEXT=.res
HELPCOMP =
HELPEXT=
HELPOUTEXT=
DEL = del
COPY = copy
MOVE = move

OUTDIRTEMP = ../wntmsc/
OUTDIRDEBUG = ../wntmsc/debug/
OUTDIRRELEASE = ../wntmsc/release/
TARGETDIR = ..\wntmsc\\
CDTOOUTDIR = cd ..\wntmsc
CDBACK = cd ..\vis
DSTLIB = omsk_wm5.lib

selected = 1
iswnt = 1

ADD_LIB=oldnames.lib comdlg32.lib comctl32.lib kernel32.lib user32.lib gdi32.lib shell32.lib msvcrt.lib 

