/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minnativehlp.h,v $
 *
 *  $Revision: 1.2 $
 *
 *	$Log: not supported by cvs2svn $
 *	Revision 1.1.1.1  2003/06/22 09:31:21  min
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
/*
	Einige Hilfsklassen zum Anbinden von native Funktionen an den Interpreter.

	Aufgeteilt:	5. 6.1999
*/

#ifndef _MINNATIVEHLP_H
#define _MINNATIVEHLP_H

#include "mstl_string.h"
#include "mstl_vector.h"

#include "minipenv.h"		// for: class minInterpreterValue;

// Hilfsklassen fuer Native-Funktions-Anbindung
// zum Umgehen der cast-Operatoren

// Klasse um eine Referenz auf einen eingebauten Datentyp zu erzeugen,
// z.B. bei f( int & i ) --> _Ref( int ) 
template <class Type>
class _Ref
{
public:
	typedef Type		buildin_cast_type;
	typedef Type		buildin_type;
	typedef Type &		buildin_ref_type;

	_Ref( const Type & aVal )
		: m_aVal( aVal )
	{
	}

	operator buildin_type ()
	{
		return m_aVal;
	}

	buildin_ref_type GetRef()
	{
		return m_aVal;
	}

private:

	Type	m_aVal;
};

// Spezialisierung fuer string Klasse, notwendig wegen cast-operator (const char *)
template <>
class _Ref<string>
{
public:
	typedef string			Type;
	typedef const char *	buildin_cast_type;
	typedef Type			buildin_type;
	typedef Type &			buildin_ref_type;

	_Ref( const Type & aVal )
		: m_aVal( aVal )
	{
	}

	operator buildin_type ()
	{
		return m_aVal.c_str();
	}

	buildin_ref_type GetRef()
	{
		return m_aVal;
	}

private:

	Type	m_aVal;
};

// Klasse um einen Value auf einen eingebauten Datentyp zu erzeugen,
// z.B. bei f( int i ) --> _Ref( int ) 
template <class Type>
class _Val
{
public:
	typedef Type		buildin_cast_type;
	typedef Type		buildin_type;
	typedef Type		buildin_ref_type;	// keine Referenz liefern !

	_Val( const Type & aVal )
		: m_aVal( aVal )
	{
	}

	operator buildin_type ()
	{
		return m_aVal;
	}

	buildin_ref_type GetRef()
	{
		return m_aVal;
	}

private:
	Type	m_aVal;
};

/*
template <>
class _Val<class Type &>
{
public:
	typedef Type		buildin_cast_type;
	typedef Type		buildin_type;
	typedef Type		buildin_ref_type;

	_Val( const Type & aVal )
		: m_aVal( aVal )
	{
	}

	operator buildin_type ()
	{
		return m_aVal;
	}

	buildin_ref_type GetRef()
	{
		return m_aVal;
	}

private:
	Type	m_aVal;
};
*/

//*************************************************************************
/** Klasse wird aufgeworfen, wenn eine Funktionalitaet noch nicht implementiert ist. */
class minNotImplementedException
{
public:
	minNotImplementedException( const string & sMsg ) : m_sMsg( sMsg ) {}
	virtual ~minNotImplementedException() {}

private:
	string		m_sMsg;
};

//*************************************************************************
// Hilfsklasse (Basisklasse zur Implementation von Funktionalitaet)
class NativeFcnWrapperBase 
{
public:
	typedef vector<bool>	ReferenceArrayType;

	NativeFcnWrapperBase( const string & sPrototype )
		: m_sPrototype( sPrototype )
	{}
	virtual ~NativeFcnWrapperBase()
	{}

	bool IsReference( int iArgIndex ) const
	{
		if( iArgIndex>=0 && iArgIndex<(int)m_aReferenceArgs.size() )
		{
			return m_aReferenceArgs[iArgIndex];
		}
		return false;
	}

	void SetReferenceArray( const ReferenceArrayType & aArray )
	{
		m_aReferenceArgs = aArray;
	}

	string GetPrototype() const
	{
		return m_sPrototype;
	}

	virtual minInterpreterValue operator()() 
	{ 
		throw minNotImplementedException( "no (0) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1 )
	{
		throw minNotImplementedException( "no (1) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1,
		minInterpreterValue & aParam2
	)
	{
		throw minNotImplementedException( "no (2) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1, 
		minInterpreterValue & aParam2, 
		minInterpreterValue & aParam3
	)
	{
		throw minNotImplementedException( "no (3) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1, 
		minInterpreterValue & aParam2, 
		minInterpreterValue & aParam3, 
		minInterpreterValue & aParam4
	)
	{
		throw minNotImplementedException( "no (4) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1, 
		minInterpreterValue & aParam2,
		minInterpreterValue & aParam3, 
		minInterpreterValue & aParam4, 
		minInterpreterValue & aParam5
	)
	{
		throw minNotImplementedException( "no (5) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1, 
		minInterpreterValue & aParam2, 
		minInterpreterValue & aParam3,
		minInterpreterValue & aParam4, 
		minInterpreterValue & aParam5, 
		minInterpreterValue & aParam6
	)
	{
		throw minNotImplementedException( "no (6) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1, 
		minInterpreterValue & aParam2, 
		minInterpreterValue & aParam3, 
		minInterpreterValue & aParam4, 
		minInterpreterValue & aParam5, 
		minInterpreterValue & aParam6,
		minInterpreterValue & aParam7
	)
	{
		throw minNotImplementedException( "no (7) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1, 
		minInterpreterValue & aParam2, 
		minInterpreterValue & aParam3, 
		minInterpreterValue & aParam4, 
		minInterpreterValue & aParam5, 
		minInterpreterValue & aParam6, 
		minInterpreterValue & aParam7, 
		minInterpreterValue & aParam8
	)
	{
		throw minNotImplementedException( "no (8) operator" ); 
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

	virtual minInterpreterValue operator()
	( 
		minInterpreterValue & aParam1, 
		minInterpreterValue & aParam2, 
		minInterpreterValue & aParam3, 
		minInterpreterValue & aParam4,
		minInterpreterValue & aParam5,
		minInterpreterValue & aParam6,
		minInterpreterValue & aParam7,
		minInterpreterValue & aParam8,
		minInterpreterValue & aParam9
	)
	{
		throw minNotImplementedException( "no (9) operator" );
		//return minInterpreterValue();	// dummy-Return fuer Compiler
	}

private:
	string							m_sPrototype;
	ReferenceArrayType				m_aReferenceArgs;
};

//*************************************************************************
template <class ReturnType>
class NativeFcnWrapper0 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType0)();

	NativeFcnWrapper0( MyFcnType0 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	minInterpreterValue operator()()
	{
		ReturnType aRet = m_aFcn();

		return aRet;
	}

	//virtual minInterpreterType	GetReturnType() const	{ return _ResolveTypeToCompileTime( (ReturnType)0 ).GetType(); }

private:
	MyFcnType0	m_aFcn;
};


//*************************************************************************
class MINDLLEXPORT NativeVoidFcnWrapper0 : public NativeFcnWrapperBase
{
public:
	typedef void (*MyFcnType0)();

	NativeVoidFcnWrapper0( MyFcnType0 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()()
	{
		m_aFcn();

		return minInterpreterValue();		// bedeutet void !
	}

private:
	MyFcnType0	m_aFcn;
};

//*************************************************************************
template <class ReturnType, class Param1Type>
class NativeFcnWrapper1 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType1)( Param1Type );

	NativeFcnWrapper1( MyFcnType1 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		ReturnType aRet = m_aFcn( aTmpParam1 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )	// es gibt immer mindestens 1 Argument !!!
			aParam1 = aTmpParam1;

		return aRet;
	}

private:
	MyFcnType1	m_aFcn;
};

//*************************************************************************
// Falls an die Funktion Referenzen uebergeben werden muessen (28.1.2003)
template <class ReturnType, class Param1Type>
class RefNativeFcnWrapper1 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType1)( typename Param1Type::buildin_ref_type );

	typedef typename Param1Type::buildin_type 		param1_type;
	typedef typename Param1Type::buildin_cast_type 	param1_buildin_cast_type;

	RefNativeFcnWrapper1( MyFcnType1 aFcn, const string & sPrototype )
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn )
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		param1_type aTmpParam1 = (param1_buildin_cast_type)aParam1;

		ReturnType aRet = m_aFcn( aTmpParam1 );

		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )	// es gibt immer mindestens 1 Argument !!!
			aParam1 = (param1_type)aTmpParam1;

		return aRet;
	}

private:
	MyFcnType1	m_aFcn;
};

//*************************************************************************
template <class Param1Type>
class NativeVoidFcnWrapper1 : public NativeFcnWrapperBase
{
public:
	typedef void (*MyFcnType1)( Param1Type );

	NativeVoidFcnWrapper1( MyFcnType1 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		m_aFcn( aTmpParam1 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )	// es gibt immer mindestens 1 Argument !!!
			aParam1 = aTmpParam1;

		return minInterpreterValue();		// bedeutet void !
	}

private:
	MyFcnType1	m_aFcn;
};

//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type>
class NativeFcnWrapper2 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType2)( Param1Type, Param2Type );

	NativeFcnWrapper2( MyFcnType2 aFcn, const string & sPrototype )
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn )
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;

		return aRet;
	}

private:
	MyFcnType2	m_aFcn;
};

//*************************************************************************
// Falls an die Funktion Referenzen uebergeben werden muessen (28.1.2003)
template <class ReturnType, class Param1Type, class Param2Type>
class RefNativeFcnWrapper2 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType2)( typename Param1Type::buildin_ref_type, typename Param2Type::buildin_ref_type );

	typedef typename Param1Type::buildin_type 			param1_type;
	typedef typename Param1Type::buildin_cast_type 		param1_buildin_cast_type;
	typedef typename Param2Type::buildin_type 			param2_type;
	typedef typename Param2Type::buildin_cast_type 		param2_buildin_cast_type;

	RefNativeFcnWrapper2( MyFcnType2 aFcn, const string & sPrototype )
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn )
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		param1_type aTmpParam1 = (param1_buildin_cast_type)aParam1;
		param2_type aTmpParam2 = (param2_buildin_cast_type)aParam2;

		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2 );

		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = (param1_type)aTmpParam1;
		if( IsReference(1) )
			aParam2 = (param2_type)aTmpParam2;

		return aRet;
	}

private:
	MyFcnType2	m_aFcn;
};

//*************************************************************************
template <class Param1Type, class Param2Type>
class NativeVoidFcnWrapper2 : public NativeFcnWrapperBase
{
public:
	typedef void (*MyFcnType2)( Param1Type, Param2Type );

	NativeVoidFcnWrapper2( MyFcnType2 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		m_aFcn( aTmpParam1, aTmpParam2 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;

		return minInterpreterValue();		// bedeutet void !
	}

private:
	MyFcnType2	m_aFcn;
};

//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type>
class NativeFcnWrapper3 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType3)( Param1Type, Param2Type, Param3Type );

	NativeFcnWrapper3( MyFcnType3 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		Param3Type aTmpParam3 = (Param3Type)aParam3;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;
		if( IsReference(2) )
			aParam3 = aTmpParam3;

		return aRet;
	}

private:
	MyFcnType3	m_aFcn;
};

//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type, class Param4Type>
class NativeFcnWrapper4 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType4)( Param1Type, Param2Type, Param3Type, Param4Type );

	NativeFcnWrapper4( MyFcnType4 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		Param3Type aTmpParam3 = (Param3Type)aParam3;
		Param4Type aTmpParam4 = (Param4Type)aParam4;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3, aTmpParam4 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;
		if( IsReference(2) )
			aParam3 = aTmpParam3;
		if( IsReference(3) )
			aParam4 = aTmpParam4;

		return aRet;
	}

private:
	MyFcnType4	m_aFcn;
};

//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type, class Param4Type, class Param5Type>
class NativeFcnWrapper5 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType5)( Param1Type, Param2Type, Param3Type, Param4Type, Param5Type );

	NativeFcnWrapper5( MyFcnType5 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		Param3Type aTmpParam3 = (Param3Type)aParam3;
		Param4Type aTmpParam4 = (Param4Type)aParam4;
		Param5Type aTmpParam5 = (Param5Type)aParam5;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3, aTmpParam4, aTmpParam5 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;
		if( IsReference(2) )
			aParam3 = aTmpParam3;
		if( IsReference(3) )
			aParam4 = aTmpParam4;
		if( IsReference(4) )
			aParam5 = aTmpParam5;

		return aRet;
	}

private:
	MyFcnType5	m_aFcn;
};

//*************************************************************************
// Falls an die Funktion Referenzen uebergeben werden muessen (28.1.2003)
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type, class Param4Type, class Param5Type>
class RefNativeFcnWrapper5 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType5)( typename Param1Type::buildin_ref_type, typename Param2Type::buildin_ref_type, typename Param3Type::buildin_ref_type, typename Param4Type::buildin_ref_type, typename Param5Type::buildin_ref_type );

	typedef typename Param1Type::buildin_type 			param1_type;
	typedef typename Param1Type::buildin_cast_type 		param1_buildin_cast_type;
	typedef typename Param2Type::buildin_type 			param2_type;
	typedef typename Param2Type::buildin_cast_type 		param2_buildin_cast_type;
	typedef typename Param3Type::buildin_type 			param3_type;
	typedef typename Param3Type::buildin_cast_type 		param3_buildin_cast_type;
	typedef typename Param4Type::buildin_type 			param4_type;
	typedef typename Param4Type::buildin_cast_type 		param4_buildin_cast_type;
	typedef typename Param5Type::buildin_type 			param5_type;
	typedef typename Param5Type::buildin_cast_type 		param5_buildin_cast_type;

	RefNativeFcnWrapper5( MyFcnType5 aFcn, const string & sPrototype )
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn )
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2,minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		param1_type aTmpParam1 = (param1_buildin_cast_type)aParam1;
		param2_type aTmpParam2 = (param2_buildin_cast_type)aParam2;
		param3_type aTmpParam3 = (param3_buildin_cast_type)aParam3;
		param4_type aTmpParam4 = (param4_buildin_cast_type)aParam4;
		param5_type aTmpParam5 = (param5_buildin_cast_type)aParam5;

		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3, aTmpParam4, aTmpParam5 );

		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = (param1_type)aTmpParam1;
		if( IsReference(1) )
			aParam2 = (param2_type)aTmpParam2;
		if( IsReference(2) )
			aParam3 = (param3_type)aTmpParam3;
		if( IsReference(3) )
			aParam4 = (param4_type)aTmpParam4;
		if( IsReference(4) )
			aParam5 = (param5_type)aTmpParam5;

		return aRet;
	}

private:
	MyFcnType5	m_aFcn;
};


//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type, class Param4Type, class Param5Type, class Param6Type>
class NativeFcnWrapper6 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType6)( Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type );

	NativeFcnWrapper6( MyFcnType6 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, 
											minInterpreterValue & aParam6 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		Param3Type aTmpParam3 = (Param3Type)aParam3;
		Param4Type aTmpParam4 = (Param4Type)aParam4;
		Param5Type aTmpParam5 = (Param5Type)aParam5;
		Param6Type aTmpParam6 = (Param6Type)aParam6;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3, aTmpParam4, aTmpParam5, aTmpParam6 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;
		if( IsReference(2) )
			aParam3 = aTmpParam3;
		if( IsReference(3) )
			aParam4 = aTmpParam4;
		if( IsReference(4) )
			aParam5 = aTmpParam5;
		if( IsReference(5) )
			aParam6 = aTmpParam6;

		return aRet;
	}

private:
	MyFcnType6	m_aFcn;
};

//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type, class Param4Type, class Param5Type, class Param6Type, class Param7Type>
class NativeFcnWrapper7 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType7)( Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type );

	NativeFcnWrapper7( MyFcnType7 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, 
											minInterpreterValue & aParam6, minInterpreterValue & aParam7 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		Param3Type aTmpParam3 = (Param3Type)aParam3;
		Param4Type aTmpParam4 = (Param4Type)aParam4;
		Param5Type aTmpParam5 = (Param5Type)aParam5;
		Param6Type aTmpParam6 = (Param6Type)aParam6;
		Param7Type aTmpParam7 = (Param7Type)aParam7;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3, aTmpParam4, aTmpParam5, aTmpParam6, aTmpParam7 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;
		if( IsReference(2) )
			aParam3 = aTmpParam3;
		if( IsReference(3) )
			aParam4 = aTmpParam4;
		if( IsReference(4) )
			aParam5 = aTmpParam5;
		if( IsReference(5) )
			aParam6 = aTmpParam6;
		if( IsReference(6) )
			aParam7 = aTmpParam7;

		return aRet;
	}

private:
	MyFcnType7	m_aFcn;
};

//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type, class Param4Type, class Param5Type, class Param6Type, class Param7Type, class Param8Type>
class NativeFcnWrapper8 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType8)( Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type, Param8Type );

	NativeFcnWrapper8( MyFcnType8 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, 
											minInterpreterValue & aParam6, minInterpreterValue & aParam7, minInterpreterValue & aParam8 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		Param3Type aTmpParam3 = (Param3Type)aParam3;
		Param4Type aTmpParam4 = (Param4Type)aParam4;
		Param5Type aTmpParam5 = (Param5Type)aParam5;
		Param6Type aTmpParam6 = (Param6Type)aParam6;
		Param7Type aTmpParam7 = (Param7Type)aParam7;
		Param8Type aTmpParam8 = (Param8Type)aParam8;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3, aTmpParam4, aTmpParam5, aTmpParam6, aTmpParam7, aTmpParam8 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;
		if( IsReference(2) )
			aParam3 = aTmpParam3;
		if( IsReference(3) )
			aParam4 = aTmpParam4;
		if( IsReference(4) )
			aParam5 = aTmpParam5;
		if( IsReference(5) )
			aParam6 = aTmpParam6;
		if( IsReference(6) )
			aParam7 = aTmpParam7;
		if( IsReference(7) )
			aParam8 = aTmpParam8;

		return aRet;
	}

private:
	MyFcnType8	m_aFcn;
};


//*************************************************************************
template <class ReturnType, class Param1Type, class Param2Type, class Param3Type, class Param4Type, class Param5Type, class Param6Type, class Param7Type, class Param8Type, class Param9Type>
class NativeFcnWrapper9 : public NativeFcnWrapperBase
{
public:
	typedef ReturnType (*MyFcnType9)( Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type, Param7Type, Param8Type, Param9Type );

	NativeFcnWrapper9( MyFcnType9 aFcn, const string & sPrototype ) 
		: NativeFcnWrapperBase( sPrototype ), m_aFcn( aFcn ) 
	{}

	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, 
											minInterpreterValue & aParam6, minInterpreterValue & aParam7, minInterpreterValue & aParam8, minInterpreterValue & aParam9 )
	{
		// benoetige Typ-Information zum Aufrufen der native-Funktion (invoke)
		Param1Type aTmpParam1 = (Param1Type)aParam1;
		Param2Type aTmpParam2 = (Param2Type)aParam2;
		Param3Type aTmpParam3 = (Param3Type)aParam3;
		Param4Type aTmpParam4 = (Param4Type)aParam4;
		Param5Type aTmpParam5 = (Param5Type)aParam5;
		Param6Type aTmpParam6 = (Param6Type)aParam6;
		Param7Type aTmpParam7 = (Param7Type)aParam7;
		Param8Type aTmpParam8 = (Param8Type)aParam8;
		Param9Type aTmpParam9 = (Param9Type)aParam9;
		ReturnType aRet = m_aFcn( aTmpParam1, aTmpParam2, aTmpParam3, aTmpParam4, aTmpParam5, aTmpParam6, aTmpParam7, aTmpParam8 );
		// falls der Parameter mit call-by-reference uebergeben wurde, muss der Wert zurueckgeschrieben werden!
		if( IsReference(0) )
			aParam1 = aTmpParam1;
		if( IsReference(1) )
			aParam2 = aTmpParam2;
		if( IsReference(2) )
			aParam3 = aTmpParam3;
		if( IsReference(3) )
			aParam4 = aTmpParam4;
		if( IsReference(4) )
			aParam5 = aTmpParam5;
		if( IsReference(5) )
			aParam6 = aTmpParam6;
		if( IsReference(6) )
			aParam7 = aTmpParam7;
		if( IsReference(7) )
			aParam8 = aTmpParam8;
		if( IsReference(8) )
			aParam9 = aTmpParam9;

		return aRet;
	}

private:
	MyFcnType9	m_aFcn;
};

#endif

