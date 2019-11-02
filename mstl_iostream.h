/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
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

// Datei zum 'mappen' von STL-Header-Dateien

#ifndef _STL_IOSTREAM_H
#define _STL_IOSTREAM_H

#ifndef __PALMOS__

#if defined( __IBMCPP__ )
#include <iostream.h>
#else
#include <iostream>
#endif

#ifdef _WIN32
using namespace std;
#endif

#else

#include <bstring.h>

class ostream
{
public:
	ostream & operator<<( ostream & (*f)( ostream & aStream ) )     { return *this; }
    ostream & operator<<( const char * )                            { return *this; }
    ostream & operator<<( int )                                     { return *this; }
    ostream & operator<<( const string & s )
    {
        *this << s.c_str();
        return *this;
    }
};

class fstream
{
};

struct loggermanip
{
	typedef ostream & (*FcnPtr)( ostream &, int );

	int nasdf;
	int m_nVal;
	FcnPtr m_f;

	loggermanip( FcnPtr f, int nVal ) : m_nVal( nVal ), m_f( f ) {}
};

ostream & endl( ostream & aLog );

inline ostream & operator<<( ostream & aLog, loggermanip & lm )
{
	return lm.m_f( aLog, lm.m_nVal );
}

class istream
{
};

extern ostream cout;
extern ostream cerr;

double atof( const char * );
long atol( const char * );
int sprintf( char *, const char * , ... );

#endif

#endif // _STL_IOSTREAM_H
