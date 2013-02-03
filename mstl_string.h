/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: e:\\home\\cvsroot/minscript/mstl_string.h,v $
 *
 *  $Revision: 1.2 $
 *
 *	$Log: mstl_string.h,v $
 *	Revision 1.2  2004/01/04 15:31:55  min
 *	Homepage link updated
 *	
 *	Revision 1.1.1.1  2003/06/22 09:31:22  min
 *	Initial checkin
 *	
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
 *  Homepage: http://www.mneuroth.de/privat/zaurus/minscript.html          *
 *                                                                         *
 ***************************************************************************/

// Datei zum 'mappen' von STL-Header-Dateien

#ifndef _STL_STRING_H
#define _STL_STRING_H

#if defined( __IBMCPP__ ) || defined( __PALMOS__ )
#include <bstring.h>
#else
#include <string>
#endif

#include <string.h>

//#ifdef _WIN32
using namespace std;
//#endif

#endif // _STL_STRING_H
