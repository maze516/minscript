minscript --makestubs cppclasstest.h --output cppclasstest_stub.cpp
minscript --makewrappers cppclasstest.h --output cppclasstest.ic
rem cl -c /ZI -GX -GR -LD -MDd -Gm -Od /FR"Debug/" /Fo"Debug/" /Fd"Debug/" /FD /GZ cppclasstest.cpp
rem cl -c /ZI -GX -GR -LD -MDd -Gm -Od /FR"Debug/" /Fo"Debug/" /Fd"Debug/" /FD /GZ cppclasstest_stub.cpp
rem cl /ZI -GX -GR -LD -MDd -Gm -Od /FR"Debug/" /Fo"Debug/" /Fd"Debug/" /FD /GZ cppclasstest_stub.obj cppclasstest.obj minscriptdll\debug\minscriptdll.lib -o cppclasstest.dll
rem minscript specs\test_dll.ic
