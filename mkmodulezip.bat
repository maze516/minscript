copy minscriptdll\release\minscriptdll.lib .
zip demo_module_win.zip mstl_*.h platform.h dllexport.h minhandle.h minscript_cpp.h minnativehlp.h minipenv.h module_test.h module_test.cpp buildmodule.ic testmodule.ic buildmodule.bat minscriptdll.lib
del minscriptdll.lib
