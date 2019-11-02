# Microsoft Developer Studio Generated NMAKE File, Based on cppclasstest.dsp
!IF "$(CFG)" == ""
CFG=CppClassTest - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. CppClassTest - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "CppClassTest - Win32 Release" && "$(CFG)" != "CppClassTest - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "cppclasstest.mak" CFG="CppClassTest - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "CppClassTest - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CppClassTest - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "CppClassTest - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cppclasstest.dll"

!ELSE 

ALL : "MinScriptDll - Win32 Release" "$(OUTDIR)\cppclasstest.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"MinScriptDll - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cppclasstest.obj"
	-@erase "$(INTDIR)\cppclasstest_stub.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cppclasstest.dll"
	-@erase "$(OUTDIR)\cppclasstest.exp"
	-@erase "$(OUTDIR)\cppclasstest.lib"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cppclasstest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\cppclasstest.pdb" /machine:I386 /out:"$(OUTDIR)\cppclasstest.dll" /implib:"$(OUTDIR)\cppclasstest.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cppclasstest.obj" \
	"$(INTDIR)\cppclasstest_stub.obj" \
	".\MinScriptDll\Release\MinScriptDll.lib"

"$(OUTDIR)\cppclasstest.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CppClassTest - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\cppclasstest.dll" "$(OUTDIR)\cppclasstest.bsc"

!ELSE 

ALL : "MinScriptDll - Win32 Debug" "$(OUTDIR)\cppclasstest.dll" "$(OUTDIR)\cppclasstest.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"MinScriptDll - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\cppclasstest.obj"
	-@erase "$(INTDIR)\cppclasstest.sbr"
	-@erase "$(INTDIR)\cppclasstest_stub.obj"
	-@erase "$(INTDIR)\cppclasstest_stub.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cppclasstest.bsc"
	-@erase "$(OUTDIR)\cppclasstest.dll"
	-@erase "$(OUTDIR)\cppclasstest.exp"
	-@erase "$(OUTDIR)\cppclasstest.ilk"
	-@erase "$(OUTDIR)\cppclasstest.lib"
	-@erase "$(OUTDIR)\cppclasstest.pdb"

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cppclasstest.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cppclasstest.sbr" \
	"$(INTDIR)\cppclasstest_stub.sbr"

"$(OUTDIR)\cppclasstest.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\cppclasstest.pdb" /debug /machine:I386 /out:"$(OUTDIR)\cppclasstest.dll" /implib:"$(OUTDIR)\cppclasstest.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cppclasstest.obj" \
	"$(INTDIR)\cppclasstest_stub.obj" \
	".\MinScriptDll\Debug\MinScriptDll.lib"

"$(OUTDIR)\cppclasstest.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cppclasstest.dep")
!INCLUDE "cppclasstest.dep"
!ELSE 
!MESSAGE Warning: cannot find "cppclasstest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CppClassTest - Win32 Release" || "$(CFG)" == "CppClassTest - Win32 Debug"
SOURCE=.\cppclasstest.cpp

!IF  "$(CFG)" == "CppClassTest - Win32 Release"


"$(INTDIR)\cppclasstest.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "CppClassTest - Win32 Debug"


"$(INTDIR)\cppclasstest.obj"	"$(INTDIR)\cppclasstest.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\cppclasstest_stub.cpp

!IF  "$(CFG)" == "CppClassTest - Win32 Release"


"$(INTDIR)\cppclasstest_stub.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "CppClassTest - Win32 Debug"


"$(INTDIR)\cppclasstest_stub.obj"	"$(INTDIR)\cppclasstest_stub.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

!IF  "$(CFG)" == "CppClassTest - Win32 Release"

"MinScriptDll - Win32 Release" : 
   cd ".\MinScriptDll"
   $(MAKE) /$(MAKEFLAGS) /F .\MinScriptDll.mak CFG="MinScriptDll - Win32 Release" 
   cd ".."

"MinScriptDll - Win32 ReleaseCLEAN" : 
   cd ".\MinScriptDll"
   $(MAKE) /$(MAKEFLAGS) /F .\MinScriptDll.mak CFG="MinScriptDll - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "CppClassTest - Win32 Debug"

"MinScriptDll - Win32 Debug" : 
   cd ".\MinScriptDll"
   $(MAKE) /$(MAKEFLAGS) /F .\MinScriptDll.mak CFG="MinScriptDll - Win32 Debug" 
   cd ".."

"MinScriptDll - Win32 DebugCLEAN" : 
   cd ".\MinScriptDll"
   $(MAKE) /$(MAKEFLAGS) /F .\MinScriptDll.mak CFG="MinScriptDll - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 


!ENDIF 

