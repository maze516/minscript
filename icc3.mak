#
# *** fuer ICC 3.x ***
#
#DEFINES = /Dgulp
DEFINES =
WARNINGS =
CCOPT = /C /Gm $(WARNINGS)
# ???
CCDLLOPT = /Ge- 
CCEXEOPT = /Ge+
DORELEASE = /O+ /Gf+ /Gi+ /G5 /D_UM_RELEASE 
DODEBUG = /Ti /D_UM_DEBUG
DOPROFILE = /Ti /Gh 
DOPROFILEOBJ = cppopa3.obj /B"/NOE"
EXEOPT = /Gm /B"/map"
# fuer Debugging: /B"/map"
DLLOPT = /Gm /Ge- /B"/map"
STDLIBS =
INCLUDES = /I../oms/stl_sgi
GRITDIR = i:\grit41
IMPLIB = implib
CDEXE = cd
CC = icc
AR = ilib
ARFIRST = 
AROPT = 
AROPTPRE = +
AROPTPOST = ,,
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
PMEXEOPT = /B"/pmtype:pm"

MONICORE = monicoreos2

RC = rc
RCOPT = -r
RCEXT=.rc
RESEXT=.res
HELPCOMP = ipfc
HELPEXT=.ipf
HELPOUTEXT=.hlp
DEL = del
COPY = copy
MOVE = move

OUTDIRTEMP = ../os2icc/
OUTDIRDEBUG = ../os2icc/debug/
OUTDIRRELEASE = ../os2icc/release/
TARGETDIR = ..\os2icc\\
CDTOOUTDIR = cd ..\os2icc
CDBACK = cd ..\vis
DSTLIB = omsk_oi3.lib

selected = 1
isos2 = 1


