# Microsoft Developer Studio Generated NMAKE File, Based on MinScriptDll.dsp
!IF "$(CFG)" == ""
CFG=MinScriptDll - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. MinScriptDll - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "MinScriptDll - Win32 Release" && "$(CFG)" != "MinScriptDll - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "MinScriptDll.mak" CFG="MinScriptDll - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "MinScriptDll - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MinScriptDll - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\MinScriptDll.dll"


CLEAN :
	-@erase "$(INTDIR)\minip.obj"
	-@erase "$(INTDIR)\minipenv.obj"
	-@erase "$(INTDIR)\minparser.obj"
	-@erase "$(INTDIR)\minparsernodes.obj"
	-@erase "$(INTDIR)\minpreproc.obj"
	-@erase "$(INTDIR)\mintokenizer.obj"
	-@erase "$(INTDIR)\osdep.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MinScriptDll.exp"
	-@erase "$(OUTDIR)\MinScriptDll.lib"
	-@erase "..\Release\MinScriptDll.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GR /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "MINSCRIPTDLL_EXPORTS" /D "_MINSCRIPT_AS_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MinScriptDll.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MinScriptDll.pdb" /machine:I386 /out:"../Release/MinScriptDll.dll" /implib:"$(OUTDIR)\MinScriptDll.lib" 
LINK32_OBJS= \
	"$(INTDIR)\minip.obj" \
	"$(INTDIR)\minipenv.obj" \
	"$(INTDIR)\minparser.obj" \
	"$(INTDIR)\minparsernodes.obj" \
	"$(INTDIR)\minpreproc.obj" \
	"$(INTDIR)\mintokenizer.obj" \
	"$(INTDIR)\osdep.obj"

"..\Release\MinScriptDll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\Debug\MinScriptDll.dll" "$(OUTDIR)\MinScriptDll.bsc"


CLEAN :
	-@erase "$(INTDIR)\minip.obj"
	-@erase "$(INTDIR)\minip.sbr"
	-@erase "$(INTDIR)\minipenv.obj"
	-@erase "$(INTDIR)\minipenv.sbr"
	-@erase "$(INTDIR)\minparser.obj"
	-@erase "$(INTDIR)\minparser.sbr"
	-@erase "$(INTDIR)\minparsernodes.obj"
	-@erase "$(INTDIR)\minparsernodes.sbr"
	-@erase "$(INTDIR)\minpreproc.obj"
	-@erase "$(INTDIR)\minpreproc.sbr"
	-@erase "$(INTDIR)\mintokenizer.obj"
	-@erase "$(INTDIR)\mintokenizer.sbr"
	-@erase "$(INTDIR)\osdep.obj"
	-@erase "$(INTDIR)\osdep.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MinScriptDll.bsc"
	-@erase "$(OUTDIR)\MinScriptDll.exp"
	-@erase "$(OUTDIR)\MinScriptDll.lib"
	-@erase "$(OUTDIR)\MinScriptDll.pdb"
	-@erase "..\Debug\MinScriptDll.dll"
	-@erase "..\Debug\MinScriptDll.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "MINSCRIPTDLL_EXPORTS" /D "_MINSCRIPT_AS_DLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MinScriptDll.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\minip.sbr" \
	"$(INTDIR)\minipenv.sbr" \
	"$(INTDIR)\minparser.sbr" \
	"$(INTDIR)\minparsernodes.sbr" \
	"$(INTDIR)\minpreproc.sbr" \
	"$(INTDIR)\mintokenizer.sbr" \
	"$(INTDIR)\osdep.sbr"

"$(OUTDIR)\MinScriptDll.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MinScriptDll.pdb" /debug /machine:I386 /out:"../Debug/MinScriptDll.dll" /implib:"$(OUTDIR)\MinScriptDll.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\minip.obj" \
	"$(INTDIR)\minipenv.obj" \
	"$(INTDIR)\minparser.obj" \
	"$(INTDIR)\minparsernodes.obj" \
	"$(INTDIR)\minpreproc.obj" \
	"$(INTDIR)\mintokenizer.obj" \
	"$(INTDIR)\osdep.obj"

"..\Debug\MinScriptDll.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("MinScriptDll.dep")
!INCLUDE "MinScriptDll.dep"
!ELSE 
!MESSAGE Warning: cannot find "MinScriptDll.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MinScriptDll - Win32 Release" || "$(CFG)" == "MinScriptDll - Win32 Debug"
SOURCE=..\minip.cpp

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"


"$(INTDIR)\minip.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"


"$(INTDIR)\minip.obj"	"$(INTDIR)\minip.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\minipenv.cpp

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"


"$(INTDIR)\minipenv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"


"$(INTDIR)\minipenv.obj"	"$(INTDIR)\minipenv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\minparser.cpp

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"


"$(INTDIR)\minparser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"


"$(INTDIR)\minparser.obj"	"$(INTDIR)\minparser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\minparsernodes.cpp

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"


"$(INTDIR)\minparsernodes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"


"$(INTDIR)\minparsernodes.obj"	"$(INTDIR)\minparsernodes.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\minpreproc.cpp

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"


"$(INTDIR)\minpreproc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"


"$(INTDIR)\minpreproc.obj"	"$(INTDIR)\minpreproc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\mintokenizer.cpp

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"


"$(INTDIR)\mintokenizer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"


"$(INTDIR)\mintokenizer.obj"	"$(INTDIR)\mintokenizer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\osdep.cpp

!IF  "$(CFG)" == "MinScriptDll - Win32 Release"


"$(INTDIR)\osdep.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MinScriptDll - Win32 Debug"


"$(INTDIR)\osdep.obj"	"$(INTDIR)\osdep.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

