/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/demomodul/init.cpp,v $
 *
 *  $Revision: 1.1 $
 *
 *	$Log: not supported by cvs2svn $
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

/*
	This is a simple demo module for minscript.

*/

#include "dllexport.h"

#include "platform.h"

#include "mstl_string.h"
#include "mstl_iostream.h"

//#include "minip.h"
//#include "minipenv.h"
#include "minnativehlp.h"

string do_something()
{
	string sBuffer( "function in the module called: do_something()" );

	return sBuffer;
}

// min todo --> hier koennte auch ein Environment-Interface ausreichen !
extern "C" bool CPPDLLEXPORT minRegisterNativeFunctions( minInterpreterEnvironmentInterface * pEnv )
{
	NativeFcnWrapperBase * pFcn = 0;

	pFcn = new NativeFcnWrapper0<string>( do_something, "string do_something();" );
	pEnv->AddNativeFunction( pFcn );

	return true;
}
