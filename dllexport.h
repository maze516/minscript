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

#ifndef _DLLEXPORT_H
#define _DLLEXPORT_H

//*************************************************************************

#if defined( _MSC_VER ) //|| defined( __MINGW32__ )
#define CPPDLLEXPORT		__declspec( dllexport )
#ifdef _MINSCRIPT_AS_DLL
#ifdef _USE_DLL
#define MINDLLEXPORT		__declspec( dllimport )
#define MINPREDLLEXPORT		__declspec( dllimport )
#else
#define MINDLLEXPORT		__declspec( dllexport )
#define MINPREDLLEXPORT		__declspec( dllexport )
#endif
#else
#define MINDLLEXPORT
#define MINPREDLLEXPORT
#endif
#define _with_bool
#define _with_iostreams
#define _with_preproc
#endif

#ifdef __IBMCPP__
#define CPPDLLEXPORT		
#ifdef _USE_DLL
#define MINDLLEXPORT
#define MINPREDLLEXPORT
#else
#define MINDLLEXPORT _Export
#define MINPREDLLEXPORT
#endif
//#define _with_bool
#define _with_iostreams
#define _with_preproc
#endif

#if defined( __linux__ ) || defined( __MINGW32__ ) || defined( __APPLE__ )
#define CPPDLLEXPORT		
#define MINDLLEXPORT
#define MINPREDLLEXPORT
#define _with_bool
#define _with_iostreams
#define _with_preproc
#endif

#ifdef __PALMOS__
#define MINDLLEXPORT
#define MINPREDLLEXPORT
//#define _with_bool
#define _with_iostreams     // dummy streams !
//#define _with_preproc
#endif

#ifdef __PIC32MX__
#define MINDLLEXPORT
#define MINPREDLLEXPORT
//#define _with_bool
#define _with_iostreams     // dummy streams !
//#define _with_preproc
#endif

#ifdef ARDUINO_ARCH_ESP32
#define MINDLLEXPORT
#define MINPREDLLEXPORT
//#define _with_bool
#define _with_iostreams     // dummy streams !
//#define _with_preproc
#endif

#endif
