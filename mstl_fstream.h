/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/mstl_fstream.h,v $
 *
 *  $Revision: 1.1.1.1 $
 *
 *	$Log: not supported by cvs2svn $
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
 *  Author:   michael.neuroth@freenet.de                                   *
 *  Homepage: http://people.freenet.de/mneuroth/zaurus/minscript.html      *
 *                                                                         *
 ***************************************************************************/

// Datei zum 'mappen' von STL-Header-Dateien

#ifndef _STL_FSTREAM_H
#define _STL_FSTREAM_H

#ifndef __PALMOS__

#if defined( __IBMCPP__ )
#include <fstream.h>
#else
#include <fstream>
#endif

#ifdef _WIN32
using namespace std;
#endif

#endif

#endif // _STL_FSTREAM_H
