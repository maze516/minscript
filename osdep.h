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

// einige plattformabhaengige Funktionen

unsigned long GetCurrentTickCount();
int WaitFcn( int nDelay );
int MySystem( const char * sCmd );

#ifdef _WIN32
#define _DIRECTORY_SEPARATOR	'\\'
#endif

#ifdef __OS2__
#define _DIRECTORY_SEPARATOR	'\\'
#endif

#if defined( __linux__ ) || defined( __APPLE__ )
#define _DIRECTORY_SEPARATOR	'/'
#endif

#ifdef __PIC32MX__
#define _DIRECTORY_SEPARATOR	'\\'
#endif

#ifdef ARDUINO_ARCH_ESP32
#define _DIRECTORY_SEPARATOR	'\\'
#endif


// separators for MINSCRIPTPATH environment variable
#ifdef _WIN32
#define PATH_SEPARATOR  ";"
#endif

#ifdef __OS2__
#define PATH_SEPARATOR	';'
#endif

#if defined( __linux__ ) || defined( __APPLE__ )
#define PATH_SEPARATOR  ":"
#endif

#ifdef __PIC32MX__
#define PATH_SEPARATOR  ";"
#endif

#ifdef ARDUINO_ARCH_ESP32
#define PATH_SEPARATOR  ";"
#endif

// ************************************************************************

long long minLoadLibrary( const char * sDllName );
bool minFreeLibrary( long long hDllModule );
void * minGetProcAddress( long long hDllModule, const char * sProcName );
