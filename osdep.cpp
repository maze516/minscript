/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2019 by Michael Neuroth
 *
 ***************************************************************************/
/***************************************************************************
 *																		   *
 * This file is part of the minscript package,                             *
 * a simple C++/Java like script interpreter.                              *
 *																		   *
 * Copyright (C) 1999-2003 by Michael Neuroth.							   *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * If you want to use this minscript package in commercial software not    *
 * conform with the GPL please contact the author.                         *
 *                                                                         *
 *  Homepage: https://github.com/mneuroth/minscript                        *
 *                                                                         *
 ***************************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __OS2__
#include <os2.h>
#endif

#if defined( __linux__ ) || defined( __APPLE__ )
#include <unistd.h>			// fuer: usleep(), execvp(), vfork()
#include <stdlib.h>			// fuer: system()
#include <time.h>			// fuer: clock()
#include <dlfcn.h>			// fuer: 
#endif

//******************************************************************
/** Funktion zur Bestimmug einer Zeitmarke */
unsigned long GetCurrentTickCount()
{
#ifdef _WIN32
	return ::GetTickCount();
#endif
#ifdef __OS2__
	ULONG nRet;
	::DosQuerySysInfo( QSV_MS_COUNT, QSV_MS_COUNT, &nRet, sizeof(nRet) );
	return nRet;
#endif
#if defined( __linux__ ) || defined( __APPLE__ )
	//static time_t g_nStartTime = time( 0 );
	//return g_nStartTime+clock();
	return (unsigned long)clock();	// siehe auch CLK_TCK und CLOCKS_PER_SEC
#endif
}

//******************************************************************
/** Funktion zum Schlafenlegen eines Threads, Angaben in ms */
int WaitFcn( int nDelay )
{
#ifdef _WIN32
	::Sleep( nDelay );
#endif
#ifdef __OS2__
	::DosSleep( nDelay );
#endif
#if defined( __linux__ ) || defined( __APPLE__ )
	::usleep( nDelay*1000 );	// delay in micro seconds
#endif
	return nDelay;
}

//******************************************************************
/** Funktion zum aufrufen von externen Programmen */
int MySystem( const char * sCmd )
{
#ifdef USEBIG
	return system( sCmd );
#else
	return -1;  // error, function not available !
#endif
}

//******************************************************************
/** Funktion zum Laden von DLLs */
long long minLoadLibrary( const char * sDllName )
{
#ifdef _WIN32
    return (long long)LoadLibrary( /*(LPCWSTR)*/sDllName );
#endif
#ifdef __IBMCPP__
	// 3.2.2000: DLL-Name immer ohne .dll Extention angeben... (Modul-Name !!!)
	char sDriveBuf[12];
	char sDirBuf[512];
	char sNameBuf[512];
	char sExtBuf[512];
	_splitpath( (char *)sDllName, sDriveBuf, sDirBuf, sNameBuf, sExtBuf );
	HMODULE hModule = 0;
	APIRET rc = DosLoadModule( 0, 0, /*sDllName*/sNameBuf, &hModule );
	//cout << "Load Lib " << sDllName << " == " << sNameBuf << " nRet=" << rc << endl;
	return (long long)hModule;
#endif
#if defined( __linux__ ) || defined( __APPLE__ )
	return (long long)0; //dlopen( sDllName, RTLD_LAZY );
#endif
}

//******************************************************************
/** Funktion zum Entladen von DLLs */
bool minFreeLibrary( long long hDllModule )
{
#ifdef _WIN32
	return FreeLibrary( (HMODULE)hDllModule )==TRUE;
#endif
#ifdef __IBMCPP__
	return DosFreeModule( (HMODULE)hDllModule )==0;
#endif
#if defined( __linux__ ) || defined( __APPLE__ )
	return 0; //dlclose( (void *)hDllModule );
#endif
}

//******************************************************************
/** Funktion zum Laden von Funktionen auf DLLs */
void * minGetProcAddress( long long hDllModule, const char * sProcName )
{
#ifdef _WIN32
	return (void *)GetProcAddress( (HMODULE)hDllModule, sProcName );
#endif
#ifdef __IBMCPP__
	minGenDllProcT pProc = 0;
	APIRET rc = DosQueryProcAddr( (HMODULE)hDllModule, 0L, sProcName, &pProc );
	return pProc;
#endif
#if defined( __linux__ ) || defined( __APPLE__ )
	return 0; //(void *)dlsym( (void *)hDllModule, sProcName );
#endif
}
