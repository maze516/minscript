copy release\minscriptdll.dll .
copy release\minscript.exe .
zip minscript_win_1_2_2.zip minscript.exe minscriptdll.dll *.html HISTORY*
del minscriptdll.dll
del minscript.exe