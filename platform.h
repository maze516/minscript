/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/platform.h,v $
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

#ifndef _PLATFORM_H
#define _PLATFORM_H

#if defined( _MSC_VER )

// lange Bezeichner-Warunung ausschalten
#pragma warning( disable : 4786 )

#define OUTPUT(x)	x
#define SMALL(x)	x
#define USEBIG

#elif defined( __IBMCPP__ )

#define OUTPUT(x)	x
#define SMALL(x)	x
#define USEBIG

#elif defined( __linux__ ) && !defined( __ZAURUS__ )

#define OUTPUT(x)	x
#define SMALL(x)	x
#define USEBIG

#elif defined( __linux__ ) && defined( __ZAURUS__ )

#define OUTPUT(x)
#define SMALL(x)
#undef USEBIG

#endif

#endif //_PLATFORM_H

