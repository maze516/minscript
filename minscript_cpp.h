/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minscript_cpp.h,v $
 *
 *  $Revision: 1.2 $
 *
 *	$Log: not supported by cvs2svn $
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

#ifndef _MINSCRIPT_ENV_H
#define _MINSCRIPT_ENV_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>

using namespace std;

inline void __println( const string & s )
{
	cout << s.c_str() << endl;
}

inline void __println( const char * s )
{
	__println( string( s ) );
}
/*
template <class MyClass>
inline string typeof( const MyClass & aObj )
{
	return "Class";
}

inline string typeof( const int & aObj )
{
	return "int";
}

inline string typeof( const double & aObj )
{
	return "double";
}

inline string typeof( const string & aObj )
{
	return "string";
}

inline string typeof( const char & aObj )
{
	return "char";
}

inline string typeof( const bool & aObj )
{
	return "bool";
}
*/
string operator+( const string & s, void * p )
{
	char sBuffer[256];

	sprintf( sBuffer, "%s0x%x", s.c_str(), p );

	return sBuffer;
}

string operator+( const string & s, int i )
{
	char sBuffer[256];

	sprintf( sBuffer, "%s%d", s.c_str(), i );

	return sBuffer;
}

string operator+( const string & s, double d )
{
	char sBuffer[256];

	sprintf( sBuffer, "%s%f", s.c_str(), d );

	return sBuffer;
}

string operator+( const string & s, bool b )
{
	char sBuffer[256];

	sprintf( sBuffer, "%s%s", s.c_str(), (b ? "true" : "false") );

	return sBuffer;
}

string operator+( const string & s, char ch )
{
	char sBuffer[256];

	sprintf( sBuffer, "%s%c", s.c_str(), ch );

	return sBuffer;
}

#endif
