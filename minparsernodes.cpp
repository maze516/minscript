/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: e:\\home\\cvsroot/minscript/minparsernodes.cpp,v $
 *
 *  $Revision: 1.2 $
 *
 *	$Log: minparsernodes.cpp,v $
 *	Revision 1.2  2004/01/04 15:54:31  min
 *	Updates for the new, optimized way to add native functions implemented
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

#include "platform.h"

#include "minparsernodes.h"
#include "minipenv.h"			// fuer: minInterpreterEnvironment

#include "minparser.h"
#include "minnativehlp.h"


#include <stdio.h>				// fuer: sprintf

//*************************************************************************

// ACHTUNG: diese implementierte Vererbungs-Hirachie entspricht nicht in allen Faellen der C++ Definition !!!
// Bemerkung:
//   Es gibt zwei Such-Modi:
//	   1) falls Methode virtuell ist, Behandlung so wie jetzt
//     2) falls Methode NICHT virtuell ist, ersetzte sClassName durch sClassScope !!!!
// Vorgehensweise:
//   Suche erst mal eine Methode im Virtuellen-Modus, ist die gefundene Methode
//   jedoch NICHT virtuell dann, muss diese Such-Funktion im anderen Modus nochmals
//   aufgerufen werden!
static string FindClassForMethod( const minInterpreterEnvironment & aEnv, const string & sClassScope, const string & sClassName, const string & sMethodName, bool bIsSearchInBaseClass, bool bTestVirtual )
{
	string sSearchClassName = sClassName;

	if( !bTestVirtual && !bIsSearchInBaseClass && (sClassScope.length()>0) && (sClassScope != sClassName) )
	{
		sSearchClassName = sClassScope;
	}

	minHandle<minClassDeclarationNode> hClass = aEnv.GetClass( sSearchClassName );
	if( hClass.IsOk() )
	{
		// Methode in der uebergebenen Klasse suchen
		minHandle<minFunctionDeclarationNode> hFunction = aEnv.GetFunction( MakeMethodName( sSearchClassName, sMethodName ) );
		if( hFunction.IsOk() )
		{
			// Bugfix 30.1.2003: Unterscheide zwei Such-Modi: virtuell und normal !
			if( bTestVirtual && !hFunction->IsVirtual() )
			{
				// nochmaliger Aufruf im nicht virtuellen Modus !
				return FindClassForMethod( aEnv, sClassScope, sClassName, sMethodName, bIsSearchInBaseClass, !bTestVirtual );
			}
			return sSearchClassName;
		}

		// Bemerkung: 
		// hier kann kein direkter, rekursiver Aufruf stehen, da sonst
		// sofort in die Tiefe gesucht werden wuerde und nicht erst in
		// der Breite, daher wird diese Rekursion manuell gesteuert !
		// ACHTUNG:
		// bei dem rekursiven Aufruf, koennte ich mir das Suchen in der
		// Klasse selbst sparen, da dies schon durchgefuehrt wurde.
		// Diese Optimierung wird zur Zeit noch nicht durchgefuehrt !

		// in der Klasse nicht gefunden, d.h. in den Basisklassen suchen (Reihenfolge entscheidet)!
		const minBaseClassList & aBaseClassList = hClass->GetAllBaseClasses();
		minBaseClassList::const_iterator aIter = aBaseClassList.begin();
		while( aIter != aBaseClassList.end() )
		{
			minHandle<minFunctionDeclarationNode> hFunction = aEnv.GetFunction( MakeMethodName( *aIter, sMethodName ) );
			if( hFunction.IsOk() )
			{
				// Bugfix 30.1.2003: Unterscheide zwei Such-Modi: virtuell und normal !
				if( bTestVirtual && !hFunction->IsVirtual() )
				{
					// nochmaliger Aufruf im nicht virtuellen Modus !
					return FindClassForMethod( aEnv, sClassScope, sClassName, sMethodName, bIsSearchInBaseClass, !bTestVirtual );
				}
				return *aIter;
			}
			++aIter;
		}

		// in den Basis-Klassen nicht gefunden, d.h. rekursiv in den Basisklassen der Basisklassen suchen (Reihenfolge entscheidet)!
		aIter = aBaseClassList.begin();
		while( aIter != aBaseClassList.end() )
		{
			const string & sTempClassName = FindClassForMethod( aEnv, sClassScope, *aIter, sMethodName, /*bIsSearchInBaseClass*/true, bTestVirtual );
			if( sTempClassName!=string( "" ) )
			{
				return sTempClassName;
			}
			++aIter;
		}
	}
	return string( "" );		// nicht gefunden !
}

// Makro-Funktion
static bool ExecuteElementOperatorHelper( int nAccessModus, minInterpreterValue & aLeftVal, minInterpreterValue & aReturnValOut, minInterpreterNode * pRightNode, minInterpreterEnvironment & aEnv )
{
	// pushe den CallStackItem des Objektes an das aEnv, 
	// damit die Member des Objektes gefunden werden koennen !
	// und fuehre dann die rechte Seite des Element-Ausdrucks aus.
	aEnv.PushCallStackItem( (*aLeftVal.GetObjectCallStackItem()) );

	bool bRet = pRightNode->Execute( nAccessModus, aReturnValOut, aEnv );

	aEnv.PopCallStackItem();

	return bRet;
}

// Makro-Funktion
static bool ExecuteConstructorHelper( const string & sClassName, minInterpreterValue & aReturnValOut, minInterpreterNode * pConstructorCall, minInterpreterEnvironment & aEnv )
{
	// erst den Konstruktor der Klasse im Environment suchen und 
	// diesen Konstruktor dann ausfuehren

	minHandle<minClassDeclarationNode> hClass = aEnv.GetClass( sClassName );

	if( !hClass.IsOk() )
	{
		// Konstruktor nicht gefunden !
		aEnv.SetError( ENV_ERROR_CLASS_NOT_FOUND, "class "+sClassName+" not found" );
		aEnv.ProcessError();
		return false;
	}

	// setzte den Klassennamen des Objektes als User-Daten am Objekt, 
	// damit die Methoden der Klasse gefunden weden !
	(*aReturnValOut.GetObjectCallStackItem())->SetUserName( sClassName );

	bool bOk = true;

	// den Call-Stack Item am Environment temporaer registrieren um dort die Variablen anzulegen !
	// Bem.: der Call-Stack Item am ReturnValOut ist das Objekt selbst !
	aEnv.PushCallStackItem( *aReturnValOut.GetObjectCallStackItem() );

	// erst die Member-Variablen des Objektes/Klasse inkl. aller Basisklassen anlegen
	hClass->InitObject( aEnv );

	// und dann den Konstruktor-Aufruf durchfuehren, inkl. aller Basisklassen-Konstruktoren...
	if( pConstructorCall )
	{
		minInterpreterValue aTempRet;
		bOk = pConstructorCall->Execute( /*nAccessModus*/0, aTempRet, aEnv );
	}
	else
	{
		// Konstruktor nicht gefunden !
		aEnv.SetError( ENV_ERROR_CONSTRUCTOR_NOT_FOUND, "constructor for class "+sClassName+" not found" );
		aEnv.ProcessError();
		return false;
	}

	// nachdem die Member-Variablen am aktuellen Objekt angelegt wurden
	// den Call-Stack Item wieder vom Environment entfernen
	aEnv.PopCallStackItem();

	return bOk;
}

// Makro-Funktion
static void ExecuteDestructorHelper( minInterpreterValue & aVar, minInterpreterEnvironment & aEnv )
{
	minHandle<minCallStackItem> hObjItem = *aVar.GetObjectCallStackItem();

	minHandle<minClassDeclarationNode> hClass = aEnv.GetClass( hObjItem->GetUserName() );

	// den Call-Stack Item am Environment temporaer registrieren um das zu zerstoerende Objekt bekannt zu machen !
	aEnv.PushCallStackItem( hObjItem );

	// jetzt den Destruktor des Objektes/Klasse aufrufen
	hClass->ExecuteDestructor( aEnv );

	// den Call-Stack Item wieder vom Environment entfernen
	aEnv.PopCallStackItem();
}

//*************************************************************************

void InitDefaultTokenizer( minTokenizer & m_aTokenizer );

//*************************************************************************
// Hilfsklasse zum Parsen von Funktions-Prototypen beim Registrieren von native Funktionen
class minPrivateParser
{
public:
	minPrivateParser()
		: m_aTokenizer(), m_aParser( &m_aTokenizer )
	{
		InitDefaultTokenizer( m_aTokenizer );

#ifndef _slow_search_
		m_aTokenizer.SortTokenContainer();
#endif
	}

	minFunctionDeclarationNode * ParseFunction( const string & sScript )
	{
		//m_aTokenizer.SetText( "{ "+sScript+" }" );	// ein Script muss immer in einem Block stehen !
		m_aTokenizer.SetText( sScript );
		if( m_aParser.ParseFunction() )
		{
			minInterpreterNode * pNode = m_aParser.GetProgramNode();
			return (minFunctionDeclarationNode *)pNode;
		}
		return 0;
	}

private:
	minTokenizer				m_aTokenizer;
	minParser					m_aParser;
};


minNativeFcnWrapperBaseAdapter::minNativeFcnWrapperBaseAdapter( NativeFcnWrapperBase * pNativeFcn )
	: m_pNativeFcn( pNativeFcn ), m_aReturnType( Unknown )
{
	// aus dem Prototyp den Namen, return-Wert und Parameter parsen
	if( m_pNativeFcn )
	{
		minPrivateParser aPrivParser;
		minFunctionDeclarationNode * pFcn = aPrivParser.ParseFunction( m_pNativeFcn->GetPrototype() );
		if( pFcn )
		{
			m_aReturnType = pFcn->GetReturnType();
			m_aVarList = pFcn->GetArgumentsList();
			m_sFcnName = pFcn->GetName();

			// set the correct infos at the native function wrapper
			NativeFcnWrapperBase::ReferenceArrayType aArgsReference(m_aVarList.size());
			for( size_t i=0; i<m_aVarList.size(); i++ )
			{
				aArgsReference[i] = m_aVarList[i]->IsReference();
			}
			m_pNativeFcn->SetReferenceArray( aArgsReference );
		}
	}
}

minNativeFcnWrapperBaseAdapter::~minNativeFcnWrapperBaseAdapter()
{
	m_aVarList.erase( m_aVarList.begin(), m_aVarList.end() );

	delete m_pNativeFcn;
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()() 
{ 
	return m_pNativeFcn->operator ()();
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()( minInterpreterValue & aParam1 )
{
	return m_pNativeFcn->operator ()( aParam1 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
( 
	minInterpreterValue & aParam1,
	minInterpreterValue & aParam2
)
{
	return m_pNativeFcn->operator ()( aParam1, aParam2 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
( 
	minInterpreterValue & aParam1, 
	minInterpreterValue & aParam2, 
	minInterpreterValue & aParam3
)
{
	return m_pNativeFcn->operator ()( aParam1, aParam2, aParam3 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
( 
	minInterpreterValue & aParam1, 
	minInterpreterValue & aParam2, 
	minInterpreterValue & aParam3, 
	minInterpreterValue & aParam4
)
{
	return m_pNativeFcn->operator ()( aParam1, aParam2, aParam3, aParam4 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
( 
	minInterpreterValue & aParam1, 
	minInterpreterValue & aParam2,
	minInterpreterValue & aParam3, 
	minInterpreterValue & aParam4, 
	minInterpreterValue & aParam5
)
{
	return m_pNativeFcn->operator ()( aParam1, aParam2, aParam3, aParam4, aParam5 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
( 
	minInterpreterValue & aParam1, 
	minInterpreterValue & aParam2, 
	minInterpreterValue & aParam3,
	minInterpreterValue & aParam4, 
	minInterpreterValue & aParam5, 
	minInterpreterValue & aParam6
)
{
	return m_pNativeFcn->operator ()( aParam1, aParam2, aParam3, aParam4, aParam5, aParam6 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
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
	return m_pNativeFcn->operator ()( aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
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
	return m_pNativeFcn->operator ()( aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7, aParam8 );
}

minInterpreterValue minNativeFcnWrapperBaseAdapter::operator()
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
	return m_pNativeFcn->operator ()( aParam1, aParam2, aParam3, aParam4, aParam5, aParam6, aParam7, aParam8, aParam9 );
}

//*************************************************************************

//*************************************************************************
//*************************************************************************
//*************************************************************************

#include "mstl_iostream.h"

template <class MyClass>
class _ValueDumper
{
public:
	_ValueDumper( const string & sMessage, MyClass & aVal ) 
		: m_sMessage( sMessage ), m_aVal( aVal ) 
	{}
	~_ValueDumper()
	{
		if( IsDebugOn() )
		{
			OUTPUT( GetDebugStream() << "DEBUG: ValueDumper: " << m_aVal << endl; )
		}
	}

private:
	string		m_sMessage;
	MyClass		m_aVal;
};

long minInterpreterNode::m_nCount = 0;
_ValueDumper<long> aValDumper( "minInterpreterNode::GetCountRef()", minInterpreterNode::GetCountRef() );

//*************************************************************************

// zum Debuggen
class MyNodeCounterDumper
{
public:
	~MyNodeCounterDumper()
	{
		if( IsDebugOn() )
		{
			OUTPUT( GetDebugStream() << "DEBUG: minInterpreterNode-Count=" << minInterpreterNode::GetCountRef() << endl; )
		}
	}
};
MyNodeCounterDumper g_aNodeDumper;

minInterpreterNode::minInterpreterNode( const string & sTypeName, minHandle<minDebuggerInfo> hDebuggerInfo )
	: m_sTypeName( sTypeName ),
	  m_hDebuggerInfo( hDebuggerInfo )
{
	++m_nCount;
}

minInterpreterNode::minInterpreterNode( const minInterpreterNode & aOther )
{
	++m_nCount;
	m_sTypeName = aOther.m_sTypeName;
	m_hDebuggerInfo = aOther.m_hDebuggerInfo;
}

minInterpreterNode::~minInterpreterNode()
{
	--m_nCount;
	if( m_nCount==0 )
	{
		if( IsDebugOn() )
		{
			OUTPUT( GetDebugStream() << "DEBUG: minInterpreterNode-Count == 0 !!!" << endl; )
		}
	}
}

int minInterpreterNode::GetLineNumber() const
{
	if (m_hDebuggerInfo.IsOk())
	{
		return m_hDebuggerInfo->iLineNo;
	}
	return 0;
}

#ifdef USEBIG
bool minInterpreterNode::Dump( ostream & aStream, const string & sSpace ) const						
{ 
	aStream << sSpace.c_str() << NODE_STRING << m_sTypeName.c_str() << endl; 
	return true;
}
#endif

bool minInterpreterNode::Execute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
    if( aEnv.IsDbgMode() )
    {
        aEnv.ProcessDbg( this );
    }
	return DoExecute( nAccessModus, aReturnValOut, aEnv );
}

bool minInterpreterNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
    return false;
}

//*************************************************************************

minForkNode::~minForkNode()
{
	delete m_pLeftNode;
	delete m_pRightNode;
}

//*************************************************************************

bool minConstantNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )	
{ 
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		aReturnValOut = m_aValue.CopyValue();	// Bugfix 10.2.2003
		return true; 
	}
	return false;
}

#ifdef USEBIG
bool minConstantNode::GenerateCppCode( string & sCodeOut )
{ 
	return m_aValue.GenerateCppCode( sCodeOut );
}
#endif

#ifdef USEBIG
bool minConstantNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << " " << m_aValue.GetInfoString().c_str() << endl;
	return true;
}
#endif

//*************************************************************************

bool minVariableNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	minHandle<minInterpreterValue> hValue = aEnv.GetValueForVariable( m_sName );

	// neu seit 4.1.2000:
	// falls Symbol nicht als Variable gefunden wurde, dann als Funktion suchen und ggf. ein Funktionsobjekt liefern! 
	if( !hValue.IsOk() )
	{
		minHandle<minFunctionDeclarationNode> hFunction = aEnv.GetFunction( m_sName );
		if( hFunction.IsOk() )
		{
			hValue.bind( new minInterpreterValue( hFunction ) );
		}
	}

	if( hValue.IsOk() )
	{
		if( IsGetLValue( nAccessModus ) )
		{
			// falls Variable const ist einen Fehler liefern !
			if( hValue->GetInterpreterType().IsConst() && (nAccessModus==/*testen*/2) )
			{
				// Bugfix 10.2.2003
				aReturnValOut = (*hValue).CopyValue();		
				return true;
				/*
				aEnv.SetError( ENV_ERROR_VAR_IS_CONST, "variable "+m_sName+" is const" );
				aEnv.ProcessError();
				return false;
				*/
				
			}
			// Referenz auf den Variablen-Wert zurueckliefern
			// Bugix: 28.1.2003: Refernz explizit setzen !
			aReturnValOut.SetRef( hValue.GetPtr(), hValue->GetInterpreterType() );
		}
		else
		{
			// Kopie des Variablen-Wertes zurueckliefern
			aReturnValOut = (*hValue).CopyValue();		// Bugfix 10.2.2003
		}
	}
	else
	{
		aEnv.SetError( ENV_ERROR_VAR_NOT_FOUND, "variable "+m_sName+" not found" );
		aEnv.ProcessError();
		return false;
	}
	return true;
}

#ifdef USEBIG
bool minVariableNode::GenerateCppCode( string & sCodeOut )			
{ 
	sCodeOut += m_sName;
	return true; 
}
#endif

#ifdef USEBIG 
bool minVariableNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << " " << m_sName.c_str() << endl;
	return true;
}
#endif

//*************************************************************************

minVariableDeclarationNode::minVariableDeclarationNode( const string & sName, minInterpreterType aType, int nArraySize,
							minInterpreterNode * pInitArrayExpression,
							minInterpreterNode * pInitExpression,
							minInterpreterNode * pConstructorCall,
							const StringListT & aTemplateTypes, 
							minHandle<minDebuggerInfo> hDebuggerInfo )
	: minInterpreterNode( _VARIABLEDECLARATIONNODE, hDebuggerInfo ),
	  m_sName( sName ), 
	  m_aType( aType ), 
	  m_nArraySize( nArraySize ), 
	  m_aClassScope( None ),
	  m_pInitArrayExpression( pInitArrayExpression ),
	  m_pInitExpression( pInitExpression ), 
	  m_pConstructorCall( pConstructorCall ),
	  m_pNextVarDecl( 0 ),
	  m_aTemplateTypes( aTemplateTypes )
{
}

minVariableDeclarationNode::~minVariableDeclarationNode()
{
	if( m_pInitArrayExpression )
		delete m_pInitArrayExpression;
	if( m_pInitExpression )
		delete m_pInitExpression;
	if( m_pConstructorCall )
		delete m_pConstructorCall;
	if( m_pNextVarDecl )
		delete m_pNextVarDecl;
}

bool minVariableDeclarationNode::IsValidMemberDeclaration() const
{ 
	return m_pInitExpression==0 && (m_pNextVarDecl ? m_pNextVarDecl->IsValidMemberDeclaration() : true); 
}

// Bem.: diese Methode und FunctionCallNode sind die einzigen Stellen im Programm in dem Variablen angelegt und am Environment registriert werden !!!
bool minVariableDeclarationNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		// Bugfix: 7.1.2003
		minInterpreterType aType( m_aType );

		if( IsArray() )
		{
			aType.SetType( Array );
		}		

		// neu seit 14.2.2003 fuer Templates
		aEnv.CheckForTemplateType( aType );

		aReturnValOut = minInterpreterValue( aType );	

		bool bOk = false;
		minHandle<minCallStackItem> hActCallStackItem = aEnv.GetActCallStackItem();
		// Unterscheidung zwischen Call by Reference und Call by Value ! (siehe auch FunctionNode)
		if( aType.IsReference() )
		{
			// hole eine Referenz auf die uebergebene Variable (im gesamten Environmet nach l-Value suchen)
			if( m_pInitExpression && m_pInitExpression->Execute( /*nAccessModus*/1, aReturnValOut, aEnv ) )
			{
				// einen neuen Interpreter-Reference-Value anlegen und diesen als lokale Variable registrieren
				bOk = hActCallStackItem->AddNewVariable( GetName(), minHandle<minInterpreterValue>( new minInterpreterValue( aReturnValOut ) ) );	
			}
			else
			{
				aEnv.SetError( ENV_ERROR_REQ_L_VALUE, "require l-value for parameter "+GetName() );
				aEnv.ProcessError();
				return false;
			}
		}
		else
		{
			// jetzt ggf. einen Initialwert fuer die Variable berechnen !
			minInterpreterValue aTempRet;
			if( !m_pInitExpression || m_pInitExpression->Execute( /*nAccessModus*/0, aTempRet, aEnv ) )
			{
				// Argument als lokale Variable anlegen (by Value), verwende Init-Ausdruck falls angegeben (Bugfix 13.12.1999)
				if( m_pInitExpression )								// Bugfix: 25.12.1999: Variable immer mit richtigen Typ anlegen !
				{
					if( !aReturnValOut.CastAndAssign( aTempRet ) )	// und Init-Typ ggf. casten !
					{
						//aEnv.SetError( ENV_ERROR_CAN_NOT_CAST, "can not cast "+(int)aTempRet.GetType()+" in "+(int)aReturnValOut.GetType() );
						aEnv.SetError( ENV_ERROR_CAN_NOT_CAST, "can not cast" );
						aEnv.ProcessError();
						return false;
					}
				}

				// seit 14.2.2003: fuer die Template Behandlung muss hier der Erzeuger des Objektes gesetzt werden !
				aReturnValOut.SetCreator( this );

				// jetzt die Variable lokal anlegen
				bOk = hActCallStackItem->AddNewVariable( GetName(), minHandle<minInterpreterValue>( new minInterpreterValue( aReturnValOut ) ) );

				// seit 16.11.1999: ggf. ein Objekt konstruieren oder Array (7.12.1999)
				if( m_aType.IsObject() || /*m_aType.*/IsArray() )
				{
					if( /*m_aType.*/IsArray() )
					{
						// hier jetzt alle Array-Elemente anlegen
						minHandle<minCallStackItem> hCallStackItem = *aReturnValOut.GetObjectCallStackItem();

						// Bugfix 7.1.2003: Variablen als Array-Dimension zulassen
						// falls Array-Size noch nicht bekannt, jetzt bestimmen
						if( m_nArraySize == UNKNOWN_ARRAY_SIZE )
						{
							minInterpreterValue aSizeVal;

							if( m_pInitArrayExpression && m_pInitArrayExpression->Execute( /*bGetLValue*/false, aSizeVal, aEnv ) )
							{
								m_nArraySize = (int)aSizeVal.GetInt();
							}
							else
							{
								m_nArraySize = 0;
							}
						}

						for( int i=0; i<m_nArraySize; i++ )
						{
							char sBuffer[c_iMaxBuffer];
							sprintf( sBuffer, "%d", i );	// lege Zahl als String an, z.B. "0", "1", etc.
							hCallStackItem->AddNewVariable( sBuffer, minHandle<minInterpreterValue>( new minInterpreterValue( m_aType ) ) );
						}
					}
					else
					{
						bOk = ExecuteConstructorHelper( m_aType.GetTypeString(), aReturnValOut, m_pConstructorCall, aEnv );
					}
				}
			}
			else
			{
				aEnv.SetError( ENV_ERROR_IN_EXPR, "error in expression before var-declaration "+GetName() );
				aEnv.ProcessError();
				return false;
			}
		}

		if( !bOk )
		{
			aEnv.SetError( ENV_ERROR_CALLING_FCN, "variable "+m_sName+" already exists or error in constructor" );
			aEnv.ProcessError();
			return false;
		}

		if( m_pNextVarDecl )
		{
			minInterpreterValue aTempRet;
			bOk = bOk && m_pNextVarDecl->Execute( /*nAccessModus*/0, aTempRet, aEnv );
		}

		return bOk;
	}
	return false;
}
	
#ifdef USEBIG
bool minVariableDeclarationNode::GenerateCppCode( string & sCodeOut )
{
	return GenerateCppCode( /*bIsFirstVar*/true, sCodeOut );
}
#endif

#ifdef USEBIG
// Bugfix 5.6.1999
bool minVariableDeclarationNode::GenerateCppCode( bool bIsFirstVar, string & sCodeOut )
{
	string sType;
	m_aType.GenerateCppCode( sType );

	// nur dies die erste Deklaration in der Variablen-Deklaration ist den Typ auch ausgeben !
	if( bIsFirstVar )
	{
		sCodeOut += sType;
	}
	sCodeOut += m_sName;
	if( m_pInitExpression )
	{
		string sTemp;
		if( !m_pInitExpression->GenerateCppCode( sTemp ) )
			return false;
		sCodeOut += " = "+sTemp;
	}

	if( m_pNextVarDecl )
	{
		string sTemp;
		if( !m_pNextVarDecl->GenerateCppCode( /*bIsFirstVar*/false, sTemp ) )		// rekursiver Aufruf !
			return false;
		sCodeOut += "," + sTemp; 
	}

	return true;
}
#endif

#ifdef USEBIG 
bool minVariableDeclarationNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << " " << m_aType.GetTypeString().c_str() << " " << GetName().c_str();
	if( m_nArraySize>0 )
	{
		aStream << "[" << m_nArraySize << "]";
	}
	if( m_pInitExpression )
	{
		aStream << " = " << endl;
		m_pInitExpression->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
	else
	{
		aStream << endl;
	}
	if( m_pNextVarDecl )
	{
		m_pNextVarDecl->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}

	return true;
}
#endif

//*************************************************************************

minClassDeclarationNode::~minClassDeclarationNode()
{
	if( m_pEnv )
	{
		m_pEnv->RemoveClass( minHandle<minClassDeclarationNode>( /*bOwner*/false, this ) );	
		// Methoden der Klasse am Envrionment entfernen
		m_pCode->UnRegisterMethods( *m_pEnv );
		// Konstruktoren der Klasse am Envrionment entfernen
		m_pCode->UnRegisterConstructorsDestructors( *m_pEnv );
	}
	delete m_pCode;
}

minInterpreterType minClassDeclarationNode::GetTemplateType( const string & sTypeName, const StringListT & aRealTypes, const minInterpreterEnvironment & aEnv, int iStackDelta ) const
{
	if( m_pTemplate )
	{
		// Anfrage an das Template-Objekt weiterleiten, das weiss was zu tun ist
		return m_pTemplate->GetTemplateType( sTypeName, aRealTypes, aEnv, iStackDelta );
	}
	return minInterpreterType();
}

bool minClassDeclarationNode::InitObject( minInterpreterEnvironment & aEnv )
{
	// erst Basisklassen initialisieren (20.1.2000)
	// das gerade initialisierende Objekt befindet sich als oberstes auf dem Callstack !
	minHandle<minCallStackItem> hObj = aEnv.GetActCallStackItem();

	minBaseClassList::const_iterator aIter = m_aBaseClassList.begin();
	while( aIter != m_aBaseClassList.end() )
	{
		// Basis-Objekt zum Initialisiern temporaer auf dem Stack ablegen
		minHandle<minCallStackItem> hBaseObj( new minCallStackItem( string("BaseClass")+g_sClassMethodSeparator+(*aIter) ) );
		aEnv.PushCallStackItem( hBaseObj );

		// Objekt vom Typ der Basisklasse erzeugen:
		minHandle<minClassDeclarationNode> hBaseClass = aEnv.GetClass( *aIter );
		hBaseClass->InitObject( aEnv );

		aEnv.PopCallStackItem();

		// fuege neu erzeugtes Basis-Objekt in das eigentliche Objekt ein
		hObj->AddBaseObject( hBaseObj );

		++aIter;
	}

	// und dann die eigenen Daten initialisieren
	bool bOk = m_pCode->InitMemberVariables( aEnv );

	return bOk;
}

bool minClassDeclarationNode::ExecuteDestructor( minInterpreterEnvironment & aEnv )
{
	bool bOk = true;

	// erst das eigentliche Objekt zerstoeren
	// baue einen Funktions-Call fuer den Destruktor-Aufruf zusammen und fuehre diesen aus !
    minFunctionCallNode * pDestructor = new minFunctionCallNode( "~"+m_sName, /*keine Argumente*/minParserItemList(), 0, /*TODO DEBUG*/0 );

	minInterpreterValue aTempVal;
	bOk = pDestructor->Execute( /*nAccessModus*/0, aTempVal, aEnv );

	delete pDestructor;

	// und dann die Basisobjekte zerstoeren
	// Base-Class Destruktoren aufrufen... (20.1.2000)
	// Bugfix 31.1.2003: jetzt die Liste von hinten aufrollen... (umgekehrte Reihenfolge wie beim Konstruktor-Aufruf)
	minBaseClassList::/*const_*/reverse_iterator aIter = m_aBaseClassList.rbegin();
	while( aIter != m_aBaseClassList.rend() )
	{
        minFunctionCallNode aBaseDestructor( "~"+(*aIter), /*keine Argumente*/minParserItemList(), 0, /*TODO DEBUG*/0 );

		minInterpreterValue aTempVal;
		/*bool bOk =*/ aBaseDestructor.Execute( /*nAccessModus*/0, aTempVal, aEnv );

		++aIter;
	}
	
	return bOk;
}

bool minClassDeclarationNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		// da die Funktion aber dem umschliessenden minBlockNode gehoert (verwaltet im m_aNodeContainer)
		// darf das Environment KEIN Eigentuemer an dieser Funktion werden.
		// WICHTIG: der minBlockNode muss natuerlich fuer die gesamte Lebensdauer des Environments
		//			die Funktion bereithalten oder falls die Funktion nicht mehr gueltig ist
		//			beim Environment deregistrieren, siehe Destuktor --> dazu Environment merken !!!
		m_pEnv = &aEnv;

		// Behandlung von Vorausdeklarationen, z.B. class MyClass;
		if( !m_pCode )
			return true;
	
		// wenn die Klasse im Script-Quellcode auftritt die Klasse nur im Environment registrieren !!!
		bool bOk1 = aEnv.AddClass( minHandle<minClassDeclarationNode>( /*bOwner*/false, this ) );	

		// und die Methoden der Klasse als Funktionen mit "Klassenname_" verziert im Environment registrieren !
		bool bOk2 = m_pCode ? m_pCode->RegisterMethods( aEnv ) : false;

		// und die Konstruktoren/Destruktor der Klasse (als Funktionen mit "Klassenname_Klassenname" verziert) im Environment registrieren !
		bool bOk3 = m_pCode ? m_pCode->RegisterConstructorsDestructors( aEnv ) : false;

		return bOk1 && bOk2 && bOk3;		
	}
	return false;
}

#ifdef USEBIG
bool minClassDeclarationNode::GenerateCppCode( string & sCodeOut )
{
// TODO
	assert( false );

	return false;
}
#endif

#ifdef USEBIG 
bool minClassDeclarationNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << " [" << GetName().c_str() << "] : " << endl;
	if( m_pCode )
	{
		m_pCode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
    return true;
}
#endif

//*************************************************************************

minClassBlockNode::minClassBlockNode( const minParserItemList & aMethodList, const minParserItemList & aVariableList, const minParserItemList & aConstructorList, minHandle<minInterpreterNode> hDestructorNode, minHandle<minDebuggerInfo> hDebuggerInfo )
    : minInterpreterNode( "ClassBlockNode", hDebuggerInfo ),
	  m_aMethodNodeContainer( aMethodList ), m_aMemberVariableNodeContainer( aVariableList ),
	  m_aConstructorNodeContainer( aConstructorList ), m_hDestructorNode( hDestructorNode )
{
}

minClassBlockNode::~minClassBlockNode()
{
}

bool minClassBlockNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	// hier ist nichts zu tun !
	return true;
}

#ifdef USEBIG
bool minClassBlockNode::GenerateCppCode( string & sCodeOut )
{
// TODO
	assert( false );

	return false;
}
#endif

#ifdef USEBIG 
bool minClassBlockNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << endl;
	// Variablen dumpen
	minParserItemList::const_iterator aIter1 = m_aMemberVariableNodeContainer.begin();
	while( aIter1 != m_aMemberVariableNodeContainer.end() )
	{
		(*aIter1)->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
		++aIter1;
	}
	// Konstruktoren dumpen
	minParserItemList::const_iterator aIter2 = m_aConstructorNodeContainer.begin();
	while( aIter2 != m_aConstructorNodeContainer.end() )
	{
		(*aIter2)->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
		++aIter2;
	}
	// Destruktor dumpen
	if( m_hDestructorNode.IsOk() )
	{
		m_hDestructorNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
	// Methoden dumpen
	minParserItemList::const_iterator aIter3 = m_aMethodNodeContainer.begin();
	while( aIter3 != m_aMethodNodeContainer.end() )
	{
		(*aIter3)->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
		++aIter3;
	}
	aStream << sSpace.c_str() << "ClassBlock done." << endl;

	return true;
}
#endif

// WICHTIG: diese Methode sollte NUR von dem minClassDeclarationNode aufgerufen werden um
//			an dem Objekt die Methoden und Variablen zu registrieren !
bool minClassBlockNode::InitMemberVariables( minInterpreterEnvironment & aEnv )
{
	bool bOk = true;

	minInterpreterValue aTempVal;
	minParserItemList::const_iterator aIter = m_aMemberVariableNodeContainer.begin();
	while( bOk && (aIter != m_aMemberVariableNodeContainer.end()) )
	{
		minHandle<minInterpreterNode> hVarNode = *aIter;
		bOk = hVarNode->Execute( /*nAccessModus*/0, aTempVal, aEnv );
		++aIter;
	}

	return bOk;
}

bool minClassBlockNode::RegisterMethods( minInterpreterEnvironment & aEnv )
{
	bool bOk = true;

	minInterpreterValue aTempVal;
	minParserItemList::const_iterator aIter = m_aMethodNodeContainer.begin();
	while( bOk && (aIter != m_aMethodNodeContainer.end()) )
	{
		minHandle<minInterpreterNode> hMethodNode = *aIter;
		bOk = hMethodNode->Execute( /*nAccessModus*/0, aTempVal, aEnv );
		++aIter;
	}

	return bOk;
}

bool minClassBlockNode::UnRegisterMethods( minInterpreterEnvironment & aEnv )
{
// TODO
	return true;
}

bool minClassBlockNode::RegisterConstructorsDestructors( minInterpreterEnvironment & aEnv )
{
	// ACHTUNG: fast gleiche Implementation wie in RegisterMethods()

	bool bOk = true;

	minInterpreterValue aTempVal;
	minParserItemList::const_iterator aIter = m_aConstructorNodeContainer.begin();
	while( bOk && (aIter != m_aConstructorNodeContainer.end()) )
	{
		minHandle<minInterpreterNode> hConstructorNode = *aIter;
		bOk = hConstructorNode->Execute( /*nAccessModus*/0, aTempVal, aEnv );
		++aIter;
	}

	if( m_hDestructorNode.IsOk() )
	{
		bOk = m_hDestructorNode->Execute( /*nAccessModus*/0, aTempVal, aEnv ) && bOk;
	}

	return bOk;
}

bool minClassBlockNode::UnRegisterConstructorsDestructors( minInterpreterEnvironment & aEnv )
{
// TODO
	return true;
}

//*************************************************************************

minTemplateNode::minTemplateNode( const StringListT & aTemplateTypes, minInterpreterNode * pClass, minHandle<minDebuggerInfo> hDebuggerInfo )
    : minInterpreterNode( "TemplateNode", hDebuggerInfo ),
	  m_aTemplateTypes( aTemplateTypes ),
	  m_pClass( pClass )
{
	if( m_pClass )
	{
		((minClassDeclarationNode *)m_pClass)->SetTemplate( this );
	}
}

minTemplateNode::~minTemplateNode()
{
	delete m_pClass;
}

minInterpreterType minTemplateNode::GetTemplateType( const string & sTypeName, const StringListT & aRealTypes, const minInterpreterEnvironment & aEnv, int iStackDelta ) const
{
	if( m_aTemplateTypes.size() == aRealTypes.size() )
	{
		StringListT::const_iterator aIter = m_aTemplateTypes.begin();
		StringListT::const_iterator aValueIter = aRealTypes.begin();

		while( aIter != m_aTemplateTypes.end() )
		{
			if( *aIter == sTypeName )
			{
				// hier findet ggf. eine rekursive Template Aufloesung statt,
				// notwendig fuer templates in templates !
				return aEnv.ResolveTypeFromString( *aValueIter, /*nextRecursiveSearchLevel*/iStackDelta+1 );
			}
			++aIter;
			++aValueIter;
		}
	}
	return minInterpreterType();
}

bool minTemplateNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	// Aufruf an die eigentliche Klasse weiterreichen
	if( m_pClass )
	{
		m_pClass->Execute( nAccessModus, aReturnValOut, aEnv );
	}

	return true;
}

#ifdef USEBIG 
bool minTemplateNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << " <";
	// Variablen dumpen
	StringListT::const_iterator aIter = m_aTemplateTypes.begin();
	while( aIter != m_aTemplateTypes.end() )
	{
		aStream << (*aIter).c_str();
		++aIter;
		if( aIter != m_aTemplateTypes.end() )
		{
			aStream << ",";
		}
	}
	aStream << ">" << endl;
	m_pClass->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	return true;
}
#endif

//*************************************************************************

void minFunctionDeclarationNode::UpdateArgumentList()
{
	// falls nur void als Argument uebergeben ist, 
	// dann dieses 'dummy' Argument jetzt wieder loeschen
	if( (m_aArgumentDeclarationList.size()==1) && (*(m_aArgumentDeclarationList.begin()))->GetType() == minInterpreterType( Void ) )
	{
		m_aArgumentDeclarationList.erase( m_aArgumentDeclarationList.begin(), m_aArgumentDeclarationList.end() );
	}
}

minFunctionDeclarationNode::~minFunctionDeclarationNode()
{
	m_aArgumentDeclarationList.erase( m_aArgumentDeclarationList.begin(), m_aArgumentDeclarationList.end() );
}

#ifdef USEBIG 
bool minFunctionDeclarationNode::Dump( ostream & aStream, const string & sSpace ) const
{
	// neu seit 29.1.2003 --> Argumente ausgeben !
	aStream << sSpace.c_str() << "Arguments:" << endl;

	minVariableDeclarationList::const_iterator aIter = m_aArgumentDeclarationList.begin();
	while( aIter != m_aArgumentDeclarationList.end() )
	{
		const minVariableDeclarationList::value_type & aVal = *aIter;

		aVal->Dump( aStream, sSpace+string( SPACE_SHIFT ) );

		++aIter;
	}

	aStream << sSpace.c_str() << "Arguments done." << endl;

    return true;
}
#endif

minParserItemList minFunctionDeclarationNode::GetInitList() const
{
	return minParserItemList();		// leere Liste liefern !
}

void minFunctionDeclarationNode::SetNewMethodName( const string & sNewName, InterpreterClassScope aClassScope )
{
	m_sName = sNewName;
	m_aClassScope = aClassScope;
}

bool minFunctionDeclarationNode::HasReferenceArgs() const
{
	minVariableDeclarationList::const_iterator aIter = m_aArgumentDeclarationList.begin();
	while( aIter != m_aArgumentDeclarationList.end() )
	{
		// const string & ist keine richtige Referenc !
		if( (*aIter)->IsReference() && !(*aIter)->IsConst() )
		{
			return true;
		}
		++aIter;
	}
	return false;
}

string minFunctionDeclarationNode::GetManglingName() const
{
	string sManglingName;	// Funktionsname gehoert nicht zum Mangling-Namen = GetName();

	const minVariableDeclarationList & aVarDeclList = GetArgumentsList();
	minVariableDeclarationList::const_iterator aIter = aVarDeclList.begin();
	if( aIter == aVarDeclList.end() )
	{
		sManglingName += g_sManglingVoid;
	}
	else
	{
		while( aIter != aVarDeclList.end() )
		{
			const minHandle<minVariableDeclarationNode> & aVarDeclNode = *aIter;
			sManglingName += g_sManglingSeparator;
			sManglingName += aVarDeclNode->GetManglingTypeName();
			++aIter;
		}
	}
	return sManglingName;
}

string minFunctionDeclarationNode::GetPrototypeString() const
{
	string sProto;

	sProto += GetReturnType().GetTypeString();
	sProto += " ";
	sProto += GetName();
	sProto += "( ";

	minVariableDeclarationList::const_iterator aIter = m_aArgumentDeclarationList.begin();
	while( aIter != m_aArgumentDeclarationList.end() )
	{
		minHandle<minVariableDeclarationNode> hVarDecl = *aIter;
		sProto += hVarDecl->GetType().GetTypeString();
		++aIter;
		if( aIter != m_aArgumentDeclarationList.end() )
		{
			sProto += ", ";
		}
	}

	sProto += " );";

	return sProto;
}

//*************************************************************************

minInterpreterFunctionDeclarationNode::~minInterpreterFunctionDeclarationNode()
{
	if( m_pEnv )
	{
		m_pEnv->RemoveFunction( minHandle<minFunctionDeclarationNode>( /*bOwner*/false, this ) );
	}
	delete m_pCode;
}

bool minInterpreterFunctionDeclarationNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		// wenn die Funktion im Script-Quellcode auftritt die Funktion nur im Environment registrieren !!!

		// da die Funktion aber dem umschliessenden minBlockNode gehoert (verwaltet im m_aNodeContainer)
		// darf das Environment KEIN Eigentuemer an dieser Funktion werden.
		// WICHTIG: der minBlockNode muss natuerlich fuer die gesamte Lebensdauer des Environments
		//			die Funktion bereithalten oder falls die Funktion nicht mehr gueltig ist
		//			beim Environment deregistrieren, siehe Destuktor --> dazu Environment merken !!!
		m_pEnv = &aEnv;

		// generate mangling string: GetArgumentsList()
#ifdef _MIN_DEBUG
//cout << "Declare: " << GetName().c_str() << " " << GetManglingName().c_str() << endl;
#endif

		// wenn kein Code fuer diese Funktion angegeben ist, dann ist es ein Funktions-Prototyp,
		// dieser wird einfach ignoriert
		if( m_pCode )
		{
			bool bOk = aEnv.AddFunction( minHandle<minFunctionDeclarationNode>( /*bOwner*/false, this ) );

			if( !bOk )
			{
				aEnv.SetError( ENV_ERROR_FCN_ALLREADY_DEFINED, "function "+GetName()+" allready defined" );
				aEnv.ProcessError();
				return false;
			}

			return bOk;
		}
		return true;
	}
	return false;
}

#ifdef USEBIG
bool minInterpreterFunctionDeclarationNode::GenerateCppCode( string & sCodeOut )
{
	string sTemp = m_aReturnType.GetTypeString();

	sCodeOut += sTemp+" "+GetName()+"(";
	// Argument Deklaration erzeugen
	minVariableDeclarationList::const_iterator aIter = m_aArgumentDeclarationList.begin();
	while( aIter != m_aArgumentDeclarationList.end() )
	{
		string sNodeStrg;

		minHandle<minVariableDeclarationNode> hNode = *aIter;
		hNode->GenerateCppCode( sNodeStrg );
		sCodeOut += sNodeStrg;
		++aIter;
		if( aIter != m_aArgumentDeclarationList.end() )
			sCodeOut += ",";
	}
	sCodeOut += ")\n";
	// Funktions-Block erzeugen
	if( m_pCode )
	{
		if( m_pCode->GenerateCppCode( sTemp ) )
		{
			sCodeOut += sTemp;
			return true;
		}
	}
	else
	{
		sCodeOut += ";";		// dann ist es ein Funktions-Prototyp
	}

	return true;
}
#endif

#ifdef USEBIG 
bool minInterpreterFunctionDeclarationNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str();
	if( IsVirtual() )
	{
		aStream << " (virtual)";
	}
	aStream << " [" << GetName().c_str() << "]";
	if( IsConstant() )
	{
		aStream << " (const)";
	}
	aStream << " : " << endl;
	minFunctionDeclarationNode::Dump( aStream, sSpace );
	if( m_pCode )
	{
		m_pCode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
    return true;
}
#endif

bool minInterpreterFunctionDeclarationNode::DoExecuteFcnBlock
(
	minInterpreterValue & aReturnValOut,
	minInterpreterEnvironment & aEnv
)
{
	return m_pCode ? m_pCode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv ) : false;
}

minParserItemList minInterpreterFunctionDeclarationNode::GetInitList() const
{
	return m_aInitList;
}

//*************************************************************************

minNativeFunctionDeclarationNode::~minNativeFunctionDeclarationNode()
{
	delete m_pFcn;
}

#ifdef USEBIG
bool minNativeFunctionDeclarationNode::GenerateCppCode( string & sCodeOut )
{
	// wird hier nicht benoetigt, da nur der Call und nicht die deklaration notwendig ist !
	// --> ggf. den Prototyp fuer die Native-Funtion angeben angeben.
	// BEMERKUNG: die native-Funktion sollte unter dem gleichen Namen im Interpreter registriert werden !!!
	return false;

	//sCodeOut += m_pFcn->GetFunctionName()+"();";		// ???
	//return true;
}
#endif

#ifdef USEBIG 
bool minNativeFunctionDeclarationNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << " [native: " << GetName().c_str() << "]" << endl;
    return true;
}
#endif

bool minNativeFunctionDeclarationNode::DoExecuteFcnBlock
(
	minInterpreterValue & aReturnValOut,
	minInterpreterEnvironment & aEnv
)
{
	bool bOk = true;

	minHandle<minCallStackItem> hActCallStackItem = aEnv.GetActCallStackItem();

	if( m_pFcn )
	{
		switch( m_pFcn->GetFunctionTyp() )
		{
			case 0 :
				aReturnValOut = (*m_pFcn)();
				break;
			case 1 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					if( hArg1.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 2 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					if( hArg1.IsOk() && hArg2.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 3 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					minHandle<minInterpreterValue> hArg3 = hActCallStackItem->GetValueForVariable( "3" );
					if( hArg1.IsOk() && hArg2.IsOk() && hArg3.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2, *hArg3 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 4 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					minHandle<minInterpreterValue> hArg3 = hActCallStackItem->GetValueForVariable( "3" );
					minHandle<minInterpreterValue> hArg4 = hActCallStackItem->GetValueForVariable( "4" );
					if( hArg1.IsOk() && hArg2.IsOk() && hArg3.IsOk() && hArg4.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2, *hArg3, *hArg4 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 5 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					minHandle<minInterpreterValue> hArg3 = hActCallStackItem->GetValueForVariable( "3" );
					minHandle<minInterpreterValue> hArg4 = hActCallStackItem->GetValueForVariable( "4" );
					minHandle<minInterpreterValue> hArg5 = hActCallStackItem->GetValueForVariable( "5" );
					if( hArg1.IsOk() && hArg2.IsOk() && hArg3.IsOk() && hArg4.IsOk() && hArg5.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2, *hArg3, *hArg4, *hArg5 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 6 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					minHandle<minInterpreterValue> hArg3 = hActCallStackItem->GetValueForVariable( "3" );
					minHandle<minInterpreterValue> hArg4 = hActCallStackItem->GetValueForVariable( "4" );
					minHandle<minInterpreterValue> hArg5 = hActCallStackItem->GetValueForVariable( "5" );
					minHandle<minInterpreterValue> hArg6 = hActCallStackItem->GetValueForVariable( "6" );
					if( hArg1.IsOk() && hArg2.IsOk() && hArg3.IsOk() && hArg4.IsOk() && hArg5.IsOk() && hArg6.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2, *hArg3, *hArg4, *hArg5, *hArg6 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 7 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					minHandle<minInterpreterValue> hArg3 = hActCallStackItem->GetValueForVariable( "3" );
					minHandle<minInterpreterValue> hArg4 = hActCallStackItem->GetValueForVariable( "4" );
					minHandle<minInterpreterValue> hArg5 = hActCallStackItem->GetValueForVariable( "5" );
					minHandle<minInterpreterValue> hArg6 = hActCallStackItem->GetValueForVariable( "6" );
					minHandle<minInterpreterValue> hArg7 = hActCallStackItem->GetValueForVariable( "7" );
					if( hArg1.IsOk() && hArg2.IsOk() && hArg3.IsOk() && hArg4.IsOk() && hArg5.IsOk() && hArg6.IsOk() && hArg7.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2, *hArg3, *hArg4, *hArg5, *hArg6, *hArg7 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 8 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					minHandle<minInterpreterValue> hArg3 = hActCallStackItem->GetValueForVariable( "3" );
					minHandle<minInterpreterValue> hArg4 = hActCallStackItem->GetValueForVariable( "4" );
					minHandle<minInterpreterValue> hArg5 = hActCallStackItem->GetValueForVariable( "5" );
					minHandle<minInterpreterValue> hArg6 = hActCallStackItem->GetValueForVariable( "6" );
					minHandle<minInterpreterValue> hArg7 = hActCallStackItem->GetValueForVariable( "7" );
					minHandle<minInterpreterValue> hArg8 = hActCallStackItem->GetValueForVariable( "8" );
					if( hArg1.IsOk() && hArg2.IsOk() && hArg3.IsOk() && hArg4.IsOk() && hArg5.IsOk() && hArg6.IsOk() && hArg7.IsOk() && hArg8.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2, *hArg3, *hArg4, *hArg5, *hArg6, *hArg7, *hArg8 );
					}
					else
					{
						bOk = false;
					}
				} break;
			case 9 :
				{
					minHandle<minInterpreterValue> hArg1 = hActCallStackItem->GetValueForVariable( "1" );
					minHandle<minInterpreterValue> hArg2 = hActCallStackItem->GetValueForVariable( "2" );
					minHandle<minInterpreterValue> hArg3 = hActCallStackItem->GetValueForVariable( "3" );
					minHandle<minInterpreterValue> hArg4 = hActCallStackItem->GetValueForVariable( "4" );
					minHandle<minInterpreterValue> hArg5 = hActCallStackItem->GetValueForVariable( "5" );
					minHandle<minInterpreterValue> hArg6 = hActCallStackItem->GetValueForVariable( "6" );
					minHandle<minInterpreterValue> hArg7 = hActCallStackItem->GetValueForVariable( "7" );
					minHandle<minInterpreterValue> hArg8 = hActCallStackItem->GetValueForVariable( "8" );
					minHandle<minInterpreterValue> hArg9 = hActCallStackItem->GetValueForVariable( "9" );
					if( hArg1.IsOk() && hArg2.IsOk() && hArg3.IsOk() && hArg4.IsOk() && hArg5.IsOk() && hArg6.IsOk() && hArg7.IsOk() && hArg8.IsOk() && hArg9.IsOk() )
					{
						aReturnValOut = (*m_pFcn)( *hArg1, *hArg2, *hArg3, *hArg4, *hArg5, *hArg6, *hArg7, *hArg8, *hArg9 );
					}
					else
					{
						bOk = false;
					}
				} break;
			default:
				// diese Stelle sollte NIE auftreten
				aEnv.SetError( ENV_UNEXPECTED_ERROR, "no such native function" );
				aEnv.ProcessError();
				bOk = false;
		}
	}
	return bOk;
}


//*************************************************************************

static _InterpreterValueContainer EvaluateArguments( minInterpreterEnvironment & aEnv, minParserItemList & aArgumentExpressionList )
{
	_InterpreterValueContainer aArgRes;

	// holle wenn moeglich ueberall Referenzen
	minParserItemList::iterator aArgIter = aArgumentExpressionList.begin();
	while( aArgIter != aArgumentExpressionList.end() )
	{
		minHandle<minInterpreterNode> hArgNode = *aArgIter;
		// hole eine Referenz auf die uebergebene Variable (falls moeglich)
		minInterpreterValue aValueRef;
		if( hArgNode->Execute( /*nAccessModus==Testen*/2, aValueRef, aEnv ) )
		{
			// Referenz gefunden, merken
			aArgRes.push_back( aValueRef );
		}
		else
		{
			// keine Referenz gefunden, Wert evaluieren, merken
			minInterpreterValue aVal;
			if( hArgNode->Execute( /*nAccessModus*/0, aVal, aEnv ) )
			{
				aArgRes.push_back( aVal );
			}
			else
			{
				// TODO: Error in FunctionCallArgument
				aArgRes.push_back( minInterpreterValue( Unknown ) );
			}
		}
		++aArgIter;
	}

	return aArgRes;
}

// baue aus dem Typ der Argumente den Mangling-String auf, z.B. %int%double&%bool*
static bool InitManglingNameCache( const _InterpreterValueContainer & aArgContainer, string & sManglingNameCache )
{
	sManglingNameCache = "";

	_InterpreterValueContainer::const_iterator aArgIter = aArgContainer.begin();
	while( aArgIter != aArgContainer.end() )
	{
		const minInterpreterValue & aArgVal = *aArgIter;
		sManglingNameCache += string( "%" )+aArgVal.GetInterpreterType().GetManglingString();	// Bugfix 10.2.2003: GetTypeString();
		++aArgIter;
	}

	if( sManglingNameCache.length()==0 )
	{
		sManglingNameCache = string( "%" ) + string( minInterpreterType( Void ).GetManglingString() );	// Bugfix 10.2.2003: GetTypeString();
	}

	return true;
}

//*************************************************************************

minFunctionCallNode::~minFunctionCallNode()
{
	// alle Elemente aus m_aArgumentExpressionList loeschen !
	m_aArgumentExpressionList.erase( m_aArgumentExpressionList.begin(), m_aArgumentExpressionList.end() );
	if( m_pFunctionExpression )
	{
		delete m_pFunctionExpression;
	}
}

/*
	Suche Funktion:
		a) gibt es nur eine, dann verwende diese.
		b) gibt es mehrere, dann fuehre Name-Mangling durch, merke den Mangling-String fue den naechsen Aufruf
		   (Mangling-Cache, der durch erstmaliges ausfuehren von Execute aktualisiert wird)
*/
bool minFunctionCallNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		string sFoundClass;

		// bestimme das oberste, sichtbare Objekt, falls es so etwas gibt,
		// anhand dieses Objektes laesst sich fest stellen ob es ggf. ein
		// Methoden-Call ist !!!
		minHandle<minCallStackItem> hLatestObjectOnStack = aEnv.GetLatestVisibleObjectOnStack();
		// und bestimme das wirklich oberste Item auf dem Stack,
		// dies muss kein Objek sein, es kann z.B. auch ein Block 
		// oder eine Funktion sein.
		minHandle<minCallStackItem> hActObj = aEnv.GetActCallStackItem();
		
		// koennte es ein Methoden-Call sein ?
		bool bIsMethod = hActObj->IsObject() || (hLatestObjectOnStack.IsOk() && hLatestObjectOnStack->IsObject());

		// wenn ja, dann sollten wir unbedingt ein passendes Objekt
		// zur Hand haben, ansonsten fehlt das this !!!
		if( bIsMethod && !hActObj->IsObject() )
		{
			hActObj = hLatestObjectOnStack;
		}

		// ACHTUNG, bis 17.1.2003 stand an dieser Stelle das Pushen des CallStackItems !

		// evaluiere alle Argumente (WICHTIG: Evaluieren der Argumente erfolg schon innerhalb der Funktion !!!)
		_InterpreterValueContainer aArgEvalContainer = EvaluateArguments( aEnv, m_aArgumentExpressionList );

		minHandle<minFunctionDeclarationNode> hFunction;
		string sFcnName;
		// ggf. in der Vererbungshirachie nach der passenden Methoden suchen
		if( bIsMethod )
		{
			// gibt es ueberhaupt an diesem Objekt (und in seinen
			// Basisklassen) eine Methode mit dem angegebenen Namen?
			// ist es eine Methode oder ein Konstruktor
			if( hActObj->GetUserName() != m_sName )
			{
				sFoundClass = FindClassForMethod( aEnv, 
						/*class-scope-name*/aEnv.GetLatestClassScopeForLatestObject(), 
						/*class-name*/hActObj->GetUserName(), 
						/*method-name*/m_sName,
						/*bIsSearchInBaseClass*/false,
						/*bTestVirtual*/true );
			}
			else
			{
				sFoundClass = hActObj->GetUserName();
			}
			sFcnName = MakeMethodName( sFoundClass, m_sName );

			// es kann nur ein Methoden-Call sein, wenn auch
			// eine passende Klasse mit der Methode gefunden
			// wurde, ansonsten kann es maximal eine einfache
			// Funktion sein, diese wird weiter unten gesucht.
			bIsMethod = (sFoundClass.length()>0);
		}
		else
		{
			sFcnName = m_sName;
		}

		// erzeuge neuen Call-Stack Eintrag im Environment, fuehre den Call aus und
		// loesche den Call-Stack Eintrag wieder !
		bool bOk = false;
		aEnv.PushCallStackItem( /*m_sName*/string( g_sFunctionCallStart )+sFcnName+"()", /*m_bHidesObject*/!bIsMethod, GetLineNumber() );		// TODO: ggf. Argumente noch angeben...

		// einen Ausdruck evaluieren (falls einer angegeben ist) der eine Funktion liefern soll (z.B. (*pFcn)();)
		if( m_pFunctionExpression )
		{
			minInterpreterValue aTemp;
			if( m_pFunctionExpression->Execute( /*nAccessModus*/0, aTemp, aEnv ) && aTemp.IsFunction() )
			{
				hFunction = aTemp.GetFunction();
			}
			else
			{
				// TODO Error
				return false;
			}
		}
		else
		{
		// TODO: hier koennte die Funktionsabfrage auch noch gecached werden !!!
			// falls es ein Methoden-Call ist, dann muss die Klasse noch VOR den eigentlichen Methoden-Namen gesetzt werden!!!
			if( aEnv.GetNoOfFunctions( sFcnName )>1 /*&& m_sName!="PrintLn"*/ )
			{
				if( !IsManglingNameCacheOk() )
				{
					InitManglingNameCache( aArgEvalContainer, m_sManglingNameCache );
				}
 				hFunction = aEnv.GetFunctionForManglingName( sFcnName, m_sManglingNameCache );

				// wenn die Funktion nicht als Methode gefunden wurde,
				// dann mal als echte Funktion probieren !
				/* ist doch nicht notwendig !
				if( !hFunction.IsOk() )
				{
					hFunction = aEnv.GetFunction( m_sName );
				}
				*/
			}
			else
			{
				hFunction = aEnv.GetFunction( sFcnName );
			}

			// neu seit 4.1.2000:
			// falls Symbol nicht als Funktion gefunden, dann als Variable mit einer Funktion suchen !
			if( !hFunction.IsOk() )
			{
				minHandle<minInterpreterValue> hValue = aEnv.GetValueForVariable( m_sName );
				if( hValue.IsOk() && hValue->IsFunction() )
				{
					hFunction = hValue->GetFunction();
				}
			}
		}

		if( hFunction.IsOk() )
		{
			// WICHTIG: Methoden duerfen nur mit einem this, d.h. an einem Objekt aufgerufen werden,
			//			d.h. MyClass_ff( 4 ); muss scheitern !
			if( hFunction->IsMethod() && !bIsMethod )
			{
				aEnv.SetError( ENV_ERROR_CALLING_METHOD, "calling method "+sFcnName+" without this" );	// m_sName
				aEnv.ProcessError();
				return false;
			}

			// hier die Aufruf-Argumente initalisieren und im CallStackItem ablegen:
			minHandle<minCallStackItem> hActCallStackItem = aEnv.GetActCallStackItem();
			const minVariableDeclarationList & aArgDeclList = hFunction->GetArgumentsList();

			// stimmt die Anzahl der uebergebenen Argumente mit der Deklaration ueberein ?
			if( aArgDeclList.size() != aArgEvalContainer.size() )
			{
				aEnv.SetError( ENV_ERROR_CALLING_FCN, "bad number of arguments in function "+sFcnName );// m_sName
				aEnv.ProcessError();
				return false;
			}

			// Argumente fuer den Aufruf an die Funktion aufbereiten
			minVariableDeclarationList::const_iterator aDeclIter = aArgDeclList.begin();
			_InterpreterValueContainer::const_iterator aArgIter = aArgEvalContainer.begin();
			int nArgCount = 0;
			while( aDeclIter != aArgDeclList.end() )
			{
				// BEMERKUNG: fuer native-Funktionen erfolgt der Zugriff auf Funktions-Argumente ueber alias-Namen !
				char sAliasName[c_iMaxBuffer];
				sprintf( sAliasName, "%d", ++nArgCount );

				minHandle<minVariableDeclarationNode> hArgDecl = *aDeclIter;
				const minInterpreterValue & aArgVal = *aArgIter;

				minInterpreterType aType = hArgDecl->GetType();

				// Unterscheidung zwischen Call by Reference und Call by Value !
				if( aType.IsReference() )
				{
					if( aArgVal.IsReference() )
					{
						// passt der formale Parameter zum uebergebenen Parameter ?
						if( aType != aArgVal.GetInterpreterType() )
						{
							aEnv.SetError( ENV_ERROR_REQ_L_VALUE, "bad reference for parameter "+hArgDecl->GetName()+" in function "+sFcnName );
							aEnv.ProcessError();
							return false;
						}

						// Argument als lokale Variable anlegen (by Reference)
						// Bugfix 29.1.2003: fuer const string & Aufrufe
						minInterpreterValue * pNewVal = new minInterpreterValue( aArgVal );
						if( hArgDecl->IsConst() )
						{
							pNewVal->GetInterpreterTypeRef().SetConst( true );
						}
						hActCallStackItem->AddNewVariable( hArgDecl->GetName(), minHandle<minInterpreterValue>( pNewVal ), sAliasName );
					}
					else
					{
						aEnv.SetError( ENV_ERROR_REQ_L_VALUE, "require l-value for parameter "+hArgDecl->GetName()+" in function "+sFcnName );
						aEnv.ProcessError();
						return false;
					}
				}
				else
				{
					// Argument als lokale Variable anlegen (by Value)
					// Bugfix 29.1.2003: fuer const string & Aufrufe

					// neu seit 14.2.2003 fuer Templates
					aEnv.CheckForTemplateType( aType );

					// Bugfix 14.2.2003: Typ darf nicht geaendert werden !!!
					minInterpreterValue * pNewVal = new minInterpreterValue( aType );
					pNewVal->CastAndAssign( aArgVal.CopyValue() );

					if( hArgDecl->IsConst() )
					{
						pNewVal->GetInterpreterTypeRef().SetConst( true );
					}
					hActCallStackItem->AddNewVariable( hArgDecl->GetName(), minHandle<minInterpreterValue>( pNewVal ), sAliasName );
				}
				++aDeclIter;
				++aArgIter;
			}

			// Bugfix 28.1.2003: bis dahin minInterpreterValue() !!!
			// ACHTUNG, hier wird der Typ der (Arbeits-)Variable fuer den 
			//          Rueckgabe-Wert geaendert !!!
			aReturnValOut.SetType( hFunction->GetReturnType() );				// default Return-Wert setzen
			// +++ fuehre jetzt Code-Block der Funktion aus +++
			try {
				// neu seit 22.1.2000
				// falls die Methode ein Konstruktor-Call ist, fuehre ggf. vorhandene
				// Initalisierungen aus

				// ist es ueberhaupt ein Construktor-Aufruf?
				if( (sFoundClass.length()>0) && (sFoundClass==m_sName) )
				{
					// hole die Liste aller Basisklassen (Kopie), hier werden 
					// alle explizit angegebenen Konstruktor-Aufrufe an Basisklassen
					// entfernt, sobald sie durchgefuehrt wurden.
					// anschliessend stehn in dieser Liste alle Klassen drin,
					// die noch keinen Konstruktoraufruf bekommen haben,
					// dafuer werden dann die Default-Konstruktoren aufgerufen
					minHandle<minClassDeclarationNode> hClass = aEnv.GetClass( sFoundClass );
					minBaseClassList aBaseClassList = hClass->GetAllBaseClasses();
	
					const minParserItemList & aInitList = hFunction->GetInitList();
					if( aInitList.size()>0 )
					{
						// fuer den Aufruf der Basisklassen-Konstruktoren das Objekt temporaer auf den Callstack ablegen
						aEnv.PushCallStackItem( hActObj );

						minParserItemList::const_iterator aIter = aInitList.begin();
						while( aIter != aInitList.end() )
						{
							minHandle<minInterpreterNode> hInitCall = *aIter;
							if( hInitCall.IsOk() )
							{
								// es muss ein FunctionCall sein, ansonsten ist
								// irgend etwas anderes schief gelaufen !!!
								// Bestimme den Namen der zu konstruierenden
								// Klasse aus dem Funktion-Namen.
								minFunctionCallNode * pConstructor = (minFunctionCallNode *)hInitCall.GetPtr();
								string sName = pConstructor->GetName();
								aBaseClassList.remove( sName );

								minInterpreterValue aTempVal;
								if( !hInitCall->Execute( /*nAccessModus*/0, aTempVal, aEnv ) )
								{
									aEnv.SetError( ENV_ERROR_IN_CONSTRUCTOR_INIT, "error in constructor init" );
									aEnv.ProcessError();
									return false;
								}
							}
							++aIter;
						}

						aEnv.PopCallStackItem();
					}

					// Bugfix 31.1.2003
					// jetzt noch die Default-Konstruktor Aufrufe fuer nicht
					// explizit angegebene Basisklassen ausfuehren
					minBaseClassList::const_iterator aBaseIter = aBaseClassList.begin();
					while( aBaseIter != aBaseClassList.end() )
					{
						minHandle<minCallStackItem> hBaseObj( new minCallStackItem( (*aBaseIter)+g_sClassMethodSeparator+(*aBaseIter) ) );
						aEnv.PushCallStackItem( hBaseObj );

                        minInterpreterNode * pBaseConstructorCall = new minFunctionCallNode( *aBaseIter, /*leere ArgumentList*/minParserItemList(), 0, /*TODO DEBUG*/0  );

						minInterpreterValue aTempRet;
						bOk = pBaseConstructorCall->Execute( /*nAccessModus*/0, aTempRet, aEnv );
						if( !bOk )
						{
							aEnv.SetError( ENV_ERROR_IN_CONSTRUCTOR_INIT, "error in constructor init for base class "+(*aBaseIter) );
							aEnv.ProcessError();
							return false;
						}

						delete pBaseConstructorCall;

						aEnv.PopCallStackItem();

						++aBaseIter;
					}
				}

				// nun die eigentliche Funktion aufrufen
				bOk = hFunction->DoExecuteFcnBlock( aReturnValOut, aEnv );
			}
			catch( minReturnException & aException )
			{
				aReturnValOut = aException.GetReturnValue();

				// 17.1.2003: ist ueberhaupt ein Return-Wert fuer diese Funktion erlaubt ?
				if( hFunction.IsOk() && 
					(hFunction->GetReturnType()==minInterpreterType( Void )) &&
					(hFunction->GetReturnType()!=aReturnValOut.GetInterpreterType()) )
				{
					aEnv.SetError( ENV_ERROR_RETURN_NOT_ALLOWED, "return not allowed in function " + hFunction->GetName() + "()" );
					aEnv.ProcessError();
					return false;
				}

				//old: aEnv.GetActCallStackItem()->SetReturnValue( aReturnValOut );
				bOk = true;
			}
			// break und continue Implementations-Exceptions duerfen
			// nicht durch Funktionen gereicht werden !
			catch( minImplException & /*aException*/ )
			{
				aEnv.SetError( ENV_ERROR_BREAK_OR_CONTINUE_NOT_ALLOWED, "continue or break not allowed" );
				aEnv.ProcessError();
				return false;
			}
		}
		else
		{
			aEnv.SetError( ENV_ERROR_FCN_NOT_FOUND, "function "+sFcnName+(m_sManglingNameCache!=string("!") ? m_sManglingNameCache : "")+"() not found" );
			aEnv.ProcessError();
			return false;
		}
		/* old
		if( bOk )
		{
			// Return-Wert am CallStack-Item setzen
			aEnv.GetActCallStackItem()->GetReturnValue( aReturnValOut );
		}
		*/
		aEnv.PopCallStackItem();
		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minFunctionCallNode::GenerateCppCode( string & sCodeOut )
{
	sCodeOut += m_sName+"(";
	// Argument Liste erzeugen
	minParserItemList::const_iterator aIter = m_aArgumentExpressionList.begin();
	while( aIter != m_aArgumentExpressionList.end() )
	{
		string sNodeStrg;

		minHandle<minInterpreterNode> hNode = *aIter;
		hNode->GenerateCppCode( sNodeStrg );
		sCodeOut += sNodeStrg;
		++aIter;
		if( aIter != m_aArgumentExpressionList.end() )
			sCodeOut += ",";
	}
	// Call-Operator abschliessen
	sCodeOut += ")";
	return true;
}
#endif

#ifdef USEBIG 
bool minFunctionCallNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << " " << m_sName.c_str() << "(...)" << endl;
	
	minParserItemList::const_iterator aIter = m_aArgumentExpressionList.begin();
	while( aIter != m_aArgumentExpressionList.end() )
	{
		(*aIter)->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
		++aIter;
	}
	
	return true;
}
#endif

//*************************************************************************

#ifdef USEBIG
bool minOperatorNode::GenerateCppCode( string & sCodeOut )
{
	if( m_pLeftNode && m_pRightNode )
	{
		string sLeft, sRight;

		if( m_pLeftNode->GenerateCppCode( sLeft ) && m_pRightNode->GenerateCppCode( sRight ) )
		{
			sCodeOut += sLeft + m_sOperator + sRight;
			return true;
		}
	}
	else if( m_pRightNode )
	{
		// dies ist ein pre-Unary-Operator !
		string sRight;

		if( m_pRightNode->GenerateCppCode( sRight ) )
		{
			sCodeOut += m_sOperator + sRight;
			return true;
		}
	}
	else if( m_pLeftNode )
	{
		// dies ist ein post-Unary-Operator !
		string sLeft;

		if( m_pLeftNode->GenerateCppCode( sLeft ) )
		{
			sCodeOut += sLeft + m_sOperator;
			return true;
		}
	}
	return false;
}
#endif

#ifdef USEBIG
bool minOperatorNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << "OperatorNode(" << m_sOperator.c_str() << ") [" << m_nOperatorLevel << "] type=" << GetClassName() << "/" << typeid(this).name() << " " << GetInfo();
	
	if( m_nOperatorLevel == UNARY_OPERATOR_LEVEL )
	{
		aStream << " [UNARY]";
	}
	aStream << endl;
	if( m_pLeftNode )
	{
		aStream << sSpace.c_str() << "L:" << endl;
		m_pLeftNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
	if( m_pRightNode )
	{
		aStream << sSpace.c_str() << "R:" << endl;
		m_pRightNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
	return true;
}
#endif

//*************************************************************************

bool minThisNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		minHandle<minCallStackItem> hLatestObjectOnStack = aEnv.GetLatestVisibleObjectOnStack();

		if( hLatestObjectOnStack.IsOk() )
		{
			// Bugfix 31.1.2003: this-Behandlung umgestellt.
			// hole das oberste (sichtbare) Objekt auf dem Stack und 
			// hole aus diesem Objekt die this-Variable, diese wird
			// ggf. on demand erzeugt (4.2.2003).
	
			minHandle<minInterpreterValue> hThis = hLatestObjectOnStack->GetThis();

			if( hThis.IsOk() )
			{
				aReturnValOut = *hThis;
				return true;
			}
			else
			{
				aEnv.SetError( ENV_ERROR_NO_THIS, "no this for object" );
				aEnv.ProcessError();
				return false;
			}
						}
		else
		{
			aEnv.SetError( ENV_ERROR_NO_THIS, "no object for this" );
			aEnv.ProcessError();
			return false;
		}
	}
	return false;
}

//*************************************************************************

// fuer Pre-Processor Implementation: existiert ein Symobl ?
bool minExistsOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		if( m_pRightNode )
		{
			if( m_pRightNode->GetClassName()==_VARIABLENODE )
			{
				minVariableNode * pVariableNode = (minVariableNode *)m_pRightNode;

				minHandle<minInterpreterValue> hValue = aEnv.GetValueForVariable( pVariableNode->GetInfo() );

				if( hValue.IsOk() )
					aReturnValOut = true;
				else
					aReturnValOut = false;

				return true;
			}
		}
	}
	return false;
}

//*************************************************************************

#ifdef USEBIG

// fuer Pre-Processor Implementation: existiert ein Symobl ?
bool minDereferenceExistsOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		if( m_pRightNode )
		{
			minInterpreterValue aRightVal;
			if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
			{
				minHandle<minInterpreterValue> hValue = aEnv.GetValueForVariable( aRightVal.GetString() );

				if( hValue.IsOk() )
					aReturnValOut = true;
				else
					aReturnValOut = false;

				return true;
			}
		}
	}
	return false;
}

//*************************************************************************

bool minTypeofOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		if( m_pRightNode )
		{
			minInterpreterValue aRightVal;
			if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
			{
				aReturnValOut = aRightVal.GetTypeString();	// liefert immer einen String !
				return true;
			}
		}
	}
	return false;
}

//*************************************************************************

bool minDebugHaltNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( m_pRightNode )
	{
		minInterpreterValue aRightVal;
		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			if( aRightVal.GetString() == "fcn" )
			{
				aEnv.DumpAllFunctions( cerr );
			}
		}
	}

	cerr << "user breakpoint" << endl;
	//cout << "user breakpoint" << endl;
	aEnv.SetError( ENV_ERROR_BREAKPOINT, "user breakpoint" );
	aEnv.ProcessError();
	return true;
}

#endif

//*************************************************************************

minNewOperatorNode::~minNewOperatorNode()
{
	if( m_pConstructorCall )
		delete m_pConstructorCall;
}

bool minNewOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		minInterpreterType aType( m_aType );

		aEnv.CheckForTemplateType( aType );

		minInterpreterValue * pNewObj = new minInterpreterValue( aType );

		// seit 14.2.2003, fuer Template Behandlung
		pNewObj->SetCreator( this );

		if( !m_aType.IsReference() )
		{
			// nach dem Anlegen eines Objektes noch den Konstruktor aufrufen
			if( m_aType.IsObject() )
			{
				ExecuteConstructorHelper( m_aType.GetTypeString(), *pNewObj, m_pConstructorCall, aEnv );
			}
			else
			{
				// einfachen Datentyp initialisieren
				if( m_pConstructorCall )
				{
					minInterpreterValue aTempRet;
					if( m_pConstructorCall->Execute( /*nAccessModus*/0, aTempRet, aEnv ) )
					{
						// neu erzeuge Variable mit dem Init-Ausdruck initialisieren
						*pNewObj = aTempRet;
					}
				}
			}
		}

		// der new-Operator liefert einen Zeiger auf den gemerkten Typ, d.h. int * fuer new int !!!
		aReturnValOut = minInterpreterValue( m_aType.GetPointerType(), pNewObj );

		return true;
	}
	return false;
}

//*************************************************************************

bool minDeleteOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pRightNode )
	{
		minInterpreterValue aRightValRef;
		if( m_pRightNode->Execute( /*nAccessModus*/1, aRightValRef, aEnv ) )		// hole eine Reference, damit der Zeiger auf 0 gesetzt werden kann !
		{
			if( aRightValRef.IsPointer() )
			{
				{
					// erst den Destruktor aufrufen (falls es ein Objekt ist)
					// Bugfix 31.1.2003 --> kein Handle verwenden !
					minInterpreterValue * pPointer = aRightValRef.GetDereference();
					if( pPointer && pPointer->IsObject() && !pPointer->IsReference() )
					{
						ExecuteDestructorHelper( *pPointer, aEnv );
					}
				}
				// und dann die Variable zerstoeren
				aRightValRef.DeletePointer();
			}
			else
			{
				aEnv.SetError( ENV_ERROR_NO_PTR, "no pointer for delete" );
				aEnv.ProcessError();
				return false;
			}
			return true;
		}
		else
		{
			aEnv.SetError( ENV_ERROR_REQ_L_VALUE, "need l-value for delete" );
			aEnv.ProcessError();
			return false;
		}
	}
	return false;
}

//*************************************************************************

bool minPointerDereferenceOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal;		

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			if( aLeftVal.IsPointer() )
			{
				minInterpreterValue * pPointer = aLeftVal.GetDereference();

				if( pPointer )
				{
					ExecuteElementOperatorHelper( nAccessModus, *pPointer, aReturnValOut, m_pRightNode, aEnv );
				}
				else
				{
					aEnv.SetError( ENV_ERROR_DEREF_NULL_PTR, "dereference on NULL pointer" );
					aEnv.ProcessError();
					return false;
				}
			}
			else
			{
				aEnv.SetError( ENV_ERROR_NO_OBJ_PTR, "no object-pointer for ptr-dereference" );
				aEnv.ProcessError();
				return false;
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minDereferenceOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( m_pRightNode )
	{
		minInterpreterValue aRightVal;
		if( m_pRightNode->Execute( nAccessModus/*0*/, aRightVal, aEnv ) )
		{
			// Spezialbehandlung fuer Funktionen durchfuehren: pFcn = fcn; ist gleichbedeutend mit pFcn = &fcn;
			if( aRightVal.IsFunction() )
			{
				aReturnValOut = minInterpreterValue( aRightVal.GetFunction() );	// Addresse von Funktion ist immer die Funktion
			}
			else
			{
				minInterpreterValue * pPointer = aRightVal.GetDereference();
				if( pPointer )
				{
					if( IsGetLValue( nAccessModus ) )
					{
						// Referenz auf Wert liefern
						// Bugix: 28.1.2003: Refernz explizit setzen !
						aReturnValOut.SetRef( pPointer, pPointer->GetInterpreterType() );
					}
					else
					{
						// Wert kopieren
						aReturnValOut = (*pPointer).CopyValue();	// Bugfix 10.2.2003
					}
				}
				else
				{
					aEnv.SetError( ENV_ERROR_DEREF_NULL_PTR, "dereference on NULL pointer" );
					aEnv.ProcessError();
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minAddressOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( m_pRightNode )
	{
		minInterpreterValue aRightVal;
		if( m_pRightNode->Execute( /*nAccessModus*/1, aRightVal, aEnv ) )
		{
			// es kommt eine Refernz zurueck, hole daraus den Zeiger auf den eigentlichen Value
			// TODO: Verbesserung hier nicht den direkten Zeiger weiter reichen, sondern einen Zeiger auf einen Zeiger,
			//		 damit das Problem von haengenden Referenzen besser geloest wird...
			//       D.h. diese Implementation ist vorlaeufig!
			// Spezialbehandlung fuer Funktionen durchfuehren: pFcn = fcn; ist gleichbedeutend mit pFcn = &fcn;
			if( aRightVal.IsFunction() )
			{
				aReturnValOut = minInterpreterValue( aRightVal.GetFunction() );	// Addresse von Funktion ist immer die Funktion
			}
			else
			{
				// Bugfix 31.1.2003: Referenz auf Zeiger liefern, nicht den Zeiger selbst, damit dieser nicht zerstoert wird !!!
				aReturnValOut = minInterpreterValue( aRightVal.GetInterpreterType().GetPointerRefType(), aRightVal.GetAddress()/*m_pToReferenceValue*/ );
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minObjectElementNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( m_pLeftNode && m_pRightNode )
	{
		SMALL( if( aEnv.IsDebugMode() ) { cout << "element-op l=" << m_pLeftNode->GetClassName() << " " << m_pLeftNode->GetInfo() << " r=" << m_pRightNode->GetClassName() << " " << m_pRightNode->GetInfo() << endl; } )

		minInterpreterValue aLeftVal;
		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			if( aLeftVal.IsObject() )
			{
				return ExecuteElementOperatorHelper( nAccessModus, aLeftVal, aReturnValOut, m_pRightNode, aEnv );
			}
			else
			{
				// es ist gar kein Objekt, das Elemente besitzt
				aEnv.SetError( ENV_ERROR_NO_OBJECT, "no object" );
				aEnv.ProcessError();
				return false;
			}
		}
	}
	return false;
}

//*************************************************************************

bool minArrayElementNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aRightVal, aLeftVal;
		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			if( aLeftVal.IsObject() || aLeftVal.IsArray() )
			{
				// bestimme den Index-Ausdruck und suche nach einem solchen Element im Objekt

				minHandle<minCallStackItem> hCallStack = *aLeftVal.GetObjectCallStackItem();

				// TODO: ggf. kann der Zugriff auf den Index verbessert/verschnellert werden !!!
				//minInterpreterVariable * pVariableOut = 0;
				string sTempIndexName = "";
				minInterpreterValue aIndexOut;
				if( m_pRightNode->Execute( /*nAccessModus*/0, aIndexOut, aEnv ) )
				{
					char sBuffer[c_iMaxBuffer];
					sprintf( sBuffer, "%ld", aIndexOut.GetInt() );
					sTempIndexName = sBuffer;
				}

				if( sTempIndexName.size()>0 )
				{
					minHandle<minInterpreterValue> hValue = hCallStack->GetValueForVariable( sTempIndexName );
					if( hValue.IsOk() )
					{
						if( IsGetLValue( nAccessModus ) )
						{
							// Referenz auf den Variablen-Wert zurueckliefern 
							// Bugix: 28.1.2003: Refernz explizit setzen !
							aReturnValOut.SetRef( hValue.GetPtr(), hValue->GetInterpreterType() );
						}
						else
						{
							// Variablen-Wert kopieren
							aReturnValOut = (*hValue).CopyValue();	// Bugfix 10.2.2003
						}
						return true;
					}
					else
					{
						// angegebener Index existiert nicht
						aEnv.SetError( ENV_ERROR_BAD_ARRAYINDEX, "bad array index" );
						aEnv.ProcessError();
						return false;
					}
				}
				else
				{
					// angegebener Index existiert nicht
					aEnv.SetError( ENV_ERROR_BAD_ARRAYINDEX, "bad array index" );
					aEnv.ProcessError();
					return false;
				}
			}
			else
			{
				// es ist gar kein Objekt, das Elemente besitzt
				aEnv.SetError( ENV_ERROR_NO_ARRAYOBJECT, "no array object" );
				aEnv.ProcessError();
				return false;
			}
		}
	}
	return false;
}

//*************************************************************************

bool minAssignOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( m_pLeftNode && m_pRightNode )
	{
		SMALL( if( aEnv.IsDebugMode() ) { cout << "assign l=" << m_pLeftNode->GetClassName() << " " << m_pLeftNode->GetInfo() << " r= " << m_pRightNode->GetClassName() << " " << m_pRightNode->GetInfo() << endl; } )

		minInterpreterValue aRightVal;
		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			// Hole Referenz auf den Wert des linken Knotens und
			// weise diesem den Wert des rechten Knotens zu ! (neu seit 23.11.1999)
			minInterpreterValue aLeftExprValueRef;
			if( m_pLeftNode->Execute( /*nAccessModus*/1, aLeftExprValueRef, aEnv ) )
			{
				if( !aLeftExprValueRef.CastAndAssign( aRightVal ) )	// Zuweisung durchfuehren aber Typ erhalten (neu seit 18.12.1999)
				{
					//aEnv.SetError( ENV_ERROR_CAN_NOT_CAST, "can not cast "+(int)aRigthVal.GetType()+" in "+(int)pLeftExprValue->GetType() );
					aEnv.SetError( ENV_ERROR_CAN_NOT_CAST, "can not cast" );
					aEnv.ProcessError();
					return false;
				}

				// der Wert einer Zuweisung ist immer der Wert der linken Seite !
				aReturnValOut = aRightVal.CopyValue();		// Bugfix 10.2.2003
				return true;
			}
			else
			{
				aEnv.SetError( ENV_ERROR_REQ_L_VALUE, "no l-value" );
				aEnv.ProcessError();
				return false;
			}
		}
	}
	return false;
}

//*************************************************************************

bool minIncOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && (m_pRightNode || m_pLeftNode) )
	{
		minInterpreterValue aRightVal;

		// der inc operator kann sowohl links (++i) als auch rechts (i++) stehen
		bool bOk = m_pRightNode ?
				   m_pRightNode->Execute( /*nAccessModus*/1, aRightVal, aEnv ) :
				   m_pLeftNode->Execute( /*nAccessModus*/1, aRightVal, aEnv );
		if( bOk )
		{
// TODO ist dies typecht ? was passiert wenn double inkrementiert wird ?
			// ist es i++ --> erst Wert liefern und dann inkrementieren
			if( m_pLeftNode )
			{
				// Kopie des Wertes zurueckliefern
				aReturnValOut = minInterpreterValue( (int)aRightVal.GetInt() );
			}
			aRightVal.CastAndAssign( minInterpreterValue( (int)(aRightVal.GetInt() + 1) ) );
			// oder ++ii --> erst inkrementieren und dann Wert liefern
			if( m_pRightNode )
			{
				aReturnValOut = minInterpreterValue( (int)aRightVal.GetInt() );
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minDecOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && (m_pRightNode || m_pLeftNode) )
	{
		minInterpreterValue aRightVal;

		// der dec operator kann sowohl links (--i) als auch rechts (i--) stehen
		bool bOk = m_pRightNode ? 
				   m_pRightNode->Execute( /*nAccessModus*/1, aRightVal, aEnv ) :
				   m_pLeftNode->Execute( /*nAccessModus*/1, aRightVal, aEnv );
		if( bOk )
		{
			// ACHTUNG: siehe Kommentar bei minIncOperatorNode
			if( m_pLeftNode )
			{
				aReturnValOut = minInterpreterValue( (int)aRightVal.GetInt() );
			}
			aRightVal.CastAndAssign( minInterpreterValue( (int)(aRightVal.GetInt() - 1) ) );
			if( m_pRightNode )
			{
				aReturnValOut = minInterpreterValue( (int)aRightVal.GetInt() );
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minNotOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pRightNode )
	{
		minInterpreterValue aRightVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			aReturnValOut = aRightVal.GetInt()==0; // old: !((bool)aRightVal.GetInt());
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minInvertOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pRightNode )
	{
		minInterpreterValue aRightVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			aReturnValOut = minInterpreterValue( (int)~aRightVal.GetInt() );
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minCommaOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		// auf jeden Fall linke und rechte Seite des Komma-Operators ausfuehren und rechte Seite als Resultat liefern
		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			aReturnValOut = aRightVal.CopyValue();		// Bugfix: 10.2.2003 
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minAddOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal;
		minInterpreterValue aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			// Spezialbehandlung des + Operators fuer Strings: ist eine der Seiten des + Operators
			// ein String, dann wird eine 'Addition' zwischen Strings durchgefuehrt (neu 18.12.1999)
			if( aLeftVal.IsString() || aRightVal.IsString() )
			{
				aReturnValOut = aLeftVal.GetString() + aRightVal.GetString();
			}
			else
			{
				// Bugfix 27.1.2003: unterscheide double und integer Addition !
				if( aLeftVal.GetType()==Int && aRightVal.GetType()==Int )
				{
					aReturnValOut = (int)aLeftVal.GetInt() + (int)aRightVal.GetInt();
				}
				else
				{
					aReturnValOut = aLeftVal.GetDouble() + aRightVal.GetDouble();
				}
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minPlusOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pRightNode )
	{
		minInterpreterValue aRightVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			if( (aRightVal.GetType() == Double) || (aRightVal.GetType() == Int) )
			{
				// beim unaeren + Operator ist nicht zu tun, einfach nur den Wert kopieren !!!
				aReturnValOut = aRightVal;
				return true;
			}
			else
			{
				// der unaere - Operator ist nur fuer Zahlen gueltig !
				return false;
			}
		}
	}
	return false;
}

//*************************************************************************

bool minSubOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			// Bugfix 27.1.2003: unterscheide double und integer Subtraktion !
			if( aLeftVal.GetType()==Int && aRightVal.GetType()==Int )
			{
				aReturnValOut = (int)aLeftVal.GetInt() - (int)aRightVal.GetInt();
			}
			else
			{
				aReturnValOut = aLeftVal.GetDouble() - aRightVal.GetDouble();
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minMinusOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pRightNode )
	{
		minInterpreterValue aRightVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			if( aRightVal.GetType() == Double )
			{
				aReturnValOut = minInterpreterValue( -aRightVal.GetDouble() );
			}
			else if( aRightVal.GetType() == Int )
			{
				aReturnValOut = minInterpreterValue( (int)-aRightVal.GetInt() );
			}
			else
			{
				// der unaere - Operator ist nur fuer Zahlen gueltig !
				return false;
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minMultOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			// Bugfix 27.1.2003: unterscheide double und integer Multiplikation !
			if( aLeftVal.GetType()==Int && aRightVal.GetType()==Int )
			{
				aReturnValOut = (int)aLeftVal.GetInt() * (int)aRightVal.GetInt();
			}
			else
			{
				aReturnValOut = aLeftVal.GetDouble() * aRightVal.GetDouble();
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minDivOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			double dDenominator = aRightVal.GetDouble();

			if( dDenominator == 0 )
			{
				aEnv.SetError( ENV_ERROR_DIV_WITH_ZERO, "div with 0" );
				aEnv.ProcessError();
				return false;
			}
			
			// Bugfix 27.1.2003: unterscheide double und integer Division !
			if( aLeftVal.GetType()==Int && aRightVal.GetType()==Int )
			{
				aReturnValOut = (int)aLeftVal.GetInt() / (int)aRightVal.GetInt();
			}
			else
			{
				aReturnValOut = dDenominator ? aLeftVal.GetDouble() / dDenominator : g_dMaxDouble;
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minModOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			aReturnValOut = (int)(aLeftVal.GetInt() % aRightVal.GetInt());
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minShiftOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			if( m_bLeftShift )
			{
				aReturnValOut = (int)(aLeftVal.GetInt() << aRightVal.GetInt());
			}
			else
			{
				aReturnValOut = (int)(aLeftVal.GetInt() >> aRightVal.GetInt());
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minLessOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aRightVal, aLeftVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) && m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			// TODO: Typ untersuchen und auf entsprechendem Typ vergleichen, z.B. string-Vergleich...
			if( m_bLessEqual )
			{
				aReturnValOut = aLeftVal.GetDouble() <= aRightVal.GetDouble();
			}
			else
			{
				aReturnValOut = aLeftVal.GetDouble() < aRightVal.GetDouble();
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minMoreOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aRightVal, aLeftVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) && m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			if( m_bMoreEqual )
			{
				aReturnValOut = aLeftVal.GetDouble() >= aRightVal.GetDouble();
			}
			else
			{
				aReturnValOut = aLeftVal.GetDouble() > aRightVal.GetDouble();
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minEqualOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aRightVal, aLeftVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) && m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			aReturnValOut = aLeftVal.GetDouble() == aRightVal.GetDouble();
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minNotEqualOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aRightVal, aLeftVal;

		if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) && m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			aReturnValOut = aLeftVal.GetDouble() != aRightVal.GetDouble();
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minLogAndOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aRightVal, aLeftVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			// short cut bei logischen Operatoren behandeln
			if( !aLeftVal.GetBool() )
			{
				aReturnValOut = false;
			}
			else
			{
				if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
				{
					aReturnValOut = /*aLeftVal.GetBool() &&*/ aRightVal.GetBool();
				}
				else
					return false;
			}
			return true;
		}
	}
	return false;
}


//*************************************************************************

bool minLogOrOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aRightVal, aLeftVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) )
		{
			// short cut bei logischen Operatoren behandeln
			if( aLeftVal.GetBool() )
			{
				aReturnValOut = true;
			}
			else
			{
				if( m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
				{
					aReturnValOut = aRightVal.GetBool();
				}
				else
					return false;
			}
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minBinAndOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			aReturnValOut = (int)(aLeftVal.GetInt() & aRightVal.GetInt());
			return true;
		}
	}
	return false;
}

//*************************************************************************

bool minBinOrOperatorNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) && m_pLeftNode && m_pRightNode )
	{
		minInterpreterValue aLeftVal, aRightVal;

		if( m_pLeftNode->Execute( /*nAccessModus*/0, aLeftVal, aEnv ) && m_pRightNode->Execute( /*nAccessModus*/0, aRightVal, aEnv ) )
		{
			aReturnValOut = (int)(aLeftVal.GetInt() | aRightVal.GetInt());
			return true;
		}
	}
	return false;
}

//*************************************************************************

minParenthisNode::~minParenthisNode()
{
	delete m_pNode;
}

bool minParenthisNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	return m_pNode->Execute( nAccessModus, aReturnValOut, aEnv );
}

#ifdef USEBIG
bool minParenthisNode::GenerateCppCode( string & sCodeOut )
{
	string sTemp, sTemp2;

	sTemp += "(";
	if( m_pNode->GenerateCppCode( sTemp2 ) )
	{
		sTemp += sTemp2;
		sTemp += ")";
		sCodeOut += sTemp;
		return true;
	}
	return false;
}
#endif

#ifdef USEBIG 
bool minParenthisNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << endl;
	m_pNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	return true;
}
#endif

//*************************************************************************

minBlockNode::~minBlockNode()
{
	// der Block-Node ist Eigentuemer an allen Knoten, diese werden automatisch ueber minHandle-Klasse freigegeben
	m_aNodeContainer.erase( m_aNodeContainer.begin(), m_aNodeContainer.end() );
}

bool minBlockNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		bool bOk = true;

		bool bNeedStackItem = NeedElementsStackItem();

		// nur bei Bedarf einen Stack-Eintrag erzeugen
		// (nur notwendig falls lokale Variable in diesem Block erzeugt werden)!
		if( bNeedStackItem )
		{
			aEnv.PushCallStackItem( "block", /*bHidesObject*/false );
		}

		try {
			minParserItemList::iterator aIter = m_aNodeContainer.begin();
			while( bOk && (aIter != m_aNodeContainer.end()) )
			{
				minHandle<minInterpreterNode> hNode = *aIter;

				// zum Debuggen die ausgefuehrten Nodes anzeigen ! (neu seit 15.11.1999) ggf. mit ifdefs schuetzen
				SMALL( if( aEnv.IsDebugMode() ) { cout << "execute node: " << hNode->GetClassName().c_str() << ": " << hNode->GetInfo() << endl; } )

				// Block ausfuehren
				bOk = hNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );		// TODO: muss hier ggf. mit einer lokalen Variablen gearbeitet werden ?
				++aIter;
			}
		}
		catch( minImplException & /*aException*/ )
		{
			// Hier Aufraeumarbeiten durchfuehren, z.B. Destruktoren aufrufen
			// und dann Exception weiter reichen.

			// WICHTIG: vor dem weiterreichen der Exception den alten aktiven Block wieder am Interpreter restaurieren
			if( bNeedStackItem )
			{
				CallDestructors( aEnv );
				aEnv.PopCallStackItem();
			}

			throw;
		}

		// Hier Aufraeumarbeiten durchfuehren, z.B. Destruktoren aufrufen
		if( bNeedStackItem )
		{
			CallDestructors( aEnv );
			aEnv.PopCallStackItem();
		}

		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minBlockNode::GenerateCppCode( string & sCodeOut )
{
	bool bOk = true;
	minParserItemList::iterator aIter = m_aNodeContainer.begin();
	sCodeOut = "{\n";
	while( bOk && (aIter != m_aNodeContainer.end()) )
	{
		string sTemp;
		minHandle<minInterpreterNode> hNode = *aIter;
		bOk = hNode->GenerateCppCode( sTemp );
		sCodeOut += sTemp+";\n";
		++aIter;
	}
	sCodeOut += "}\n";

	return bOk;
}
#endif

#ifdef USEBIG
bool minBlockNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << endl;
	minParserItemList::const_iterator aIter = m_aNodeContainer.begin();
	while( aIter != m_aNodeContainer.end() )
	{
		(*aIter)->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
		++aIter;
	}
	aStream << sSpace.c_str() << "Block done." << endl;
	return false;
}
#endif

const minParserItemList & minBlockNode::GetAllNodes() const
{
	return m_aNodeContainer;
}

bool minBlockNode::NeedElementsStackItem() const
{
	minParserItemList::const_iterator aIter = m_aNodeContainer.begin();
	while( aIter != m_aNodeContainer.end() )	// Bugfix: 23.12.1999 begin() --> end() !!!
	{
		// sobald nur ein Element aus der Statement-List einen Stack-Eintrag
		// benoetigt liefere true zurueck !
		if( (*aIter)->NeedStackItem() )
			return true;
		++aIter;								// Bugfix: 23.12.1999
	}
	return false;
}

bool minBlockNode::CallDestructors( minInterpreterEnvironment & aEnv )
{
	minHandle<minCallStackItem> hActCallStackItem = aEnv.GetActCallStackItem();

	// Bugfix 5.2.2003: Destruktoren in umgekehrter Reihenfolge wie beim Anlegen aufrufen!
	minCallStackItem::VariableContainerT::reverse_iterator aIter = hActCallStackItem->m_aVariableContainer.rbegin();
	while( aIter != hActCallStackItem->m_aVariableContainer.rend() )
	{
		minInterpreterVariable & aVar = *aIter;
		if( aVar.GetValue()->IsObject() && !aVar.GetValue()->IsReference() )	// Bugfix: 27.12.1999 bei Referenzen KEIN Destruktor aufrufen !
		{
			ExecuteDestructorHelper( *(aVar.GetValue()), aEnv );
		}
		++aIter;
	}

	return true;
}

//*************************************************************************

minTypedefNode::~minTypedefNode()
{
	if( m_pClassDeclarationNode )
		delete m_pClassDeclarationNode;
}

bool minTypedefNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	// hier ist nichts zu tun, da alles schon im Parser erledigt wird (Typedef-Ersetzungs-Liste)
	// Ausnahme: es ist eine struct/class-Deklaration enthalten, diese muss nun ausgefuehrt werden !
	if( m_pClassDeclarationNode )
	{
		return m_pClassDeclarationNode->Execute( nAccessModus, aReturnValOut, aEnv );
	}
	return true;
}

#ifdef USEBIG
bool minTypedefNode::GenerateCppCode( string & sCodeOut )
{
	sCodeOut += "typedef ";
	if( m_pClassDeclarationNode )
	{
		string sTemp;
		m_pClassDeclarationNode->GenerateCppCode( sTemp );
		sCodeOut += sTemp;
	}
	else
	{
		sCodeOut += m_aOldType.GetTypeString();
	}
	sCodeOut += " ";
	sCodeOut += m_sNewTokenString;
	sCodeOut += ";\n";
	return false;
}
#endif


//*************************************************************************

minCaseLabelNode::~minCaseLabelNode()
{
	delete m_pConstExpressionNode;
}

bool minCaseLabelNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		bool bOk = true;

		minParserItemList::iterator aIter = m_aStatementList.begin();
		while( bOk && (aIter != m_aStatementList.end()) )
		{
			minHandle<minInterpreterNode> hNode = *aIter;

			// zum Debuggen die ausgefuehrten Nodes anzeigen ! (neu seit 15.11.1999) ggf. mit ifdefs schuetzen
			SMALL( if( aEnv.IsDebugMode() ) { cout << "execute node: " << hNode->GetClassName().c_str() << ": " << hNode->GetInfo() << endl; } )

			// Block ausfuehren
			bOk = hNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );		// TODO: muss hier ggf. mit einer lokalen Variablen gearbeitet werden ?
			++aIter;
		}

		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minCaseLabelNode::GenerateCppCode( string & sCodeOut )
{
	return false;
}
#endif

#ifdef USEBIG 
bool minCaseLabelNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << endl;
	return true;
}
#endif

int minCaseLabelNode::GetConstantValue( minInterpreterEnvironment & aEnv ) const
{
	if( m_pConstExpressionNode )
	{
		minInterpreterValue aVal;

		m_pConstExpressionNode->Execute( /*nAccessModus*/0, aVal, aEnv );

		return (int)aVal.GetInt();
	}
	return -1;
}

bool minCaseLabelNode::IsDefaultLabel() const
{
	return (m_pConstExpressionNode==0);
}

//*************************************************************************

minSwitchNode::~minSwitchNode()
{
	delete m_pExpressionNode;
}

bool minSwitchNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		bool bOk;
		minInterpreterValue aExprVal;

		if( (bOk=m_pExpressionNode->Execute( /*nAccessModus*/0, aExprVal, aEnv )) )
		{
			try {
				minHandle<minInterpreterNode> hDefaultNode;
				bool bFound = false;
				int iValue	= (int)aExprVal.GetInt();


				minParserItemList::iterator aIter = m_aCaseLabelList.begin();
				while( bOk && (aIter != m_aCaseLabelList.end()) )
				{
					int iTest = -1;
					minHandle<minInterpreterNode> hNode = *aIter;

					// ggf. dynamic_cast verwenden
					const minCaseLabelNode * pCaseNode = (const minCaseLabelNode *)hNode.GetPtr();

					if( pCaseNode )
					{
						iTest = pCaseNode->GetConstantValue( aEnv );
						
						// merke den default-Label, falls kein case Label trifft
						if( pCaseNode->IsDefaultLabel() )
						{
							hDefaultNode = hNode;
						}
					}

					// ist dieser case label gefragt ?
					if( bFound || (iValue == iTest) )
					{
						bFound = true;

						// zum Debuggen die ausgefuehrten Nodes anzeigen ! 
						SMALL( if( aEnv.IsDebugMode() ) { cout << "execute node: " << hNode->GetClassName().c_str() << ": " << hNode->GetInfo() << endl; } )

						// alle Statements nach dem case-Label ausfuehren
						bOk = hNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );
					}
					++aIter;
				}

				// muss ggf. der default-Label angewendet werden?
				if( !bFound && hDefaultNode.IsOk() )
				{
					// zum Debuggen die ausgefuehrten Nodes anzeigen ! 
					SMALL( if( aEnv.IsDebugMode() )	{ cout << "execute node: " << hDefaultNode->GetClassName().c_str() << ": " << hDefaultNode->GetInfo() << endl; } )

					hDefaultNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );
				}
			}
			catch( minBreakException & /*aException*/ )
			{
				bOk = true;
			}
		}
		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minSwitchNode::GenerateCppCode( string & sCodeOut )
{
	string sTempExpr, sTempStatement;
/*
	if( m_pExpressionNode->GenerateCppCode( sTempExpr ) && m_pStatementNode->GenerateCppCode( sTempStatement ) )
	{
		sCodeOut += "switch( ";
		sCodeOut += sTempExpr;
		sCodeOut += " )\n";
		sCodeOut += sTempStatement;
		return true;
	}
*/	return false;
}
#endif

#ifdef USEBIG 
bool minSwitchNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << endl;
	if( m_pExpressionNode )
	{
		m_pExpressionNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
/*	if( m_pStatementNode )
	{
		m_pStatementNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
*/
	return true;
}
#endif

//*************************************************************************

minWhileNode::~minWhileNode()
{
	delete m_pExpressionNode;
	delete m_pStatementNode;
}

bool minWhileNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		bool bOk;
		minInterpreterValue aExprVal;

		while( (bOk=m_pExpressionNode->Execute( /*nAccessModus*/0, aExprVal, aEnv )) && aExprVal.GetInt() )
		{
			try {
				bOk = m_pStatementNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );
			}
			catch( minContinueException & /*aException*/ )
			{
				continue;
			}
			catch( minBreakException & /*aException*/ )
			{
				bOk = true;
				break;
			}
		}
		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minWhileNode::GenerateCppCode( string & sCodeOut )
{
	string sTempExpr, sTempStatement;

	if( m_pExpressionNode->GenerateCppCode( sTempExpr ) && m_pStatementNode->GenerateCppCode( sTempStatement ) )
	{
		sCodeOut += "while( ";
		sCodeOut += sTempExpr;
		sCodeOut += " )\n";
		sCodeOut += sTempStatement;
		return true;
	}
	return false;
}
#endif

#ifdef USEBIG 
bool minWhileNode::Dump( ostream & aStream, const string & sSpace ) const
{
	aStream << sSpace.c_str() << NODE_STRING << GetClassName().c_str() << endl;
	if( m_pExpressionNode )
	{
		m_pExpressionNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}
	if( m_pStatementNode )
	{
		m_pStatementNode->Dump( aStream, sSpace+string( SPACE_SHIFT ) );
	}

	return true;
}
#endif

//*************************************************************************

minDoNode::~minDoNode()
{
	delete m_pExpressionNode;
	delete m_pStatementNode;
}

bool minDoNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		bool bOk;
		minInterpreterValue aExprVal;

		do {
			try {
				bOk = m_pStatementNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );
			}
			catch( minContinueException & /*aException*/ )
			{
				continue;
			}
			catch( minBreakException & /*aException*/ )
			{
				bOk = true;
				break;
			}
		} while( (bOk=m_pExpressionNode->Execute( /*nAccessModus*/0, aExprVal, aEnv )) && aExprVal.GetInt() );
		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minDoNode::GenerateCppCode( string & sCodeOut )
{
	string sTempExpr, sTempStatement;

	if( m_pExpressionNode->GenerateCppCode( sTempExpr ) && m_pStatementNode->GenerateCppCode( sTempStatement ) )
	{
		sCodeOut += "do ";
		sCodeOut += sTempStatement;
		sCodeOut += "while( ";
		sCodeOut += sTempExpr;
		sCodeOut += " );\n";
		return true;
	}
	return false;
}
#endif

//*************************************************************************

minForNode::~minForNode()
{
	delete m_pInitExpressionNode;
	delete m_pCheckExpressionNode;
	delete m_pLoopExpressionNode;
	delete m_pStatementNode;
}

bool minForNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		bool bOk;
		bool bContinue = true;

		if( (bOk=m_pInitExpressionNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv )) )
		{
			minInterpreterValue aLoopExprVal;

			while( bContinue && (bOk=m_pCheckExpressionNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv )) && aReturnValOut.GetInt() )
			{
				try {
					bOk = m_pStatementNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );
				}
				catch( minContinueException & /*aException*/ )
				{
				}
				catch( minBreakException & /*aException*/ )
				{
					bOk = true;
					break;
				}
				bContinue = m_pLoopExpressionNode->Execute( /*nAccessModus*/0, aLoopExprVal, aEnv );
			}
		}
		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minForNode::GenerateCppCode( string & sCodeOut )
{
	string sTempInitExpr, sTempCheckExpr, sTempLoopExpr, sTempStatement;

	if( m_pInitExpressionNode->GenerateCppCode( sTempInitExpr ) &&
		m_pCheckExpressionNode->GenerateCppCode( sTempCheckExpr ) &&
		m_pLoopExpressionNode->GenerateCppCode( sTempLoopExpr ) &&
		m_pStatementNode->GenerateCppCode( sTempStatement ) )
	{
		sCodeOut += "for( ";
		sCodeOut += sTempInitExpr;
		sCodeOut += "; ";
		sCodeOut += sTempCheckExpr;
		sCodeOut += "; ";
		sCodeOut += sTempLoopExpr;
		sCodeOut += " )\n";
		sCodeOut += sTempStatement;
		sCodeOut += ";\n";
		return true;
	}
	return false;
}
#endif

//*************************************************************************

minIfNode::~minIfNode()
{
	delete m_pExpressionNode;
	delete m_pThenStatementNode;
	delete m_pElseStatementNode;
}

bool minIfNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		bool bOk;

		if( (bOk=m_pExpressionNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv )) && aReturnValOut.GetInt() )
		{
			bOk = m_pThenStatementNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );
		}
		else if( m_pElseStatementNode )
		{
			bOk = m_pElseStatementNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv );
		}
		return bOk;
	}
	return false;
}

#ifdef USEBIG
bool minIfNode::GenerateCppCode( string & sCodeOut )
{
	string sTempExpr, sTempThenStatement, sTempElseStatement;

	if( m_pExpressionNode->GenerateCppCode( sTempExpr ) &&
		m_pThenStatementNode->GenerateCppCode( sTempThenStatement ) &&
		(!m_pElseStatementNode || m_pElseStatementNode->GenerateCppCode( sTempElseStatement ) ) )
	{
		sCodeOut += "if( ";
		sCodeOut += sTempExpr;
		sCodeOut += " )\n";
		sCodeOut += sTempThenStatement;
		sCodeOut += ";\n";
		if( m_pElseStatementNode )
		{
			sCodeOut += "else\n";
			sCodeOut += sTempElseStatement;
			sCodeOut += ";\n";
		}
		return true;
	}
	return false;
}
#endif

//*************************************************************************

minSizeofNode::~minSizeofNode()
{
	delete m_pExpressionNode;
}

bool minSizeofNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		// FRAGE: Darf man hier den Call-By-Reference Parameter als lokale Variable verwenden ?
		//		  Oder gibt es da einige Seiteneffekte ?
		if( m_pExpressionNode && m_pExpressionNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv ) )
		{
			aReturnValOut = aReturnValOut.GetSizeofValue();
			return true;
		}
	}
	return false;
}

#ifdef USEBIG
bool minSizeofNode::GenerateCppCode( string & sCodeOut )
{
	assert( false );

	return false;
}
#endif

//*************************************************************************

minReturnNode::~minReturnNode()
{
	delete m_pExpressionNode;
}

bool minReturnNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		// FRAGE: Darf man hier den Call-By-Reference Parameter als lokale Variable verwenden ?
		//		  Oder gibt es da einige Seiteneffekte ?
		if( m_pExpressionNode && m_pExpressionNode->Execute( /*nAccessModus*/0, aReturnValOut, aEnv ) )
		{
			// Return-Wert an der Exception setzten
			throw minReturnException( aReturnValOut );
			return true;
		}
		// void als Return-Wert setzen
		throw minReturnException( minInterpreterValue() );
		return true;
	}
	return false;
}

#ifdef USEBIG
bool minReturnNode::GenerateCppCode( string & sCodeOut )
{
	string sTemp;

	sCodeOut += "return";
	if( m_pExpressionNode && m_pExpressionNode->GenerateCppCode( sTemp ) )
	{
		sCodeOut += " "+sTemp;
	}
	sCodeOut += ";\n";
	return true;
}
#endif

//*************************************************************************

bool minBreakNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		// breche gerade aktive Loop (while, do, for) durch Aufwerfen einer Exception ab,
		// diese Exception wird von den Loop-Behandlungsroutinen abgefangen und bearbeitet !
		throw minBreakException();
		return true;
	}
	return false;
}

#ifdef USEBIG
bool minBreakNode::GenerateCppCode( string & sCodeOut )
{
	sCodeOut += "break;";
	return true;
}
#endif

//*************************************************************************

bool minContinueNode::DoExecute( int nAccessModus, minInterpreterValue & aReturnValOut, minInterpreterEnvironment & aEnv )
{
	if( CheckLValueNode( nAccessModus, aEnv ) )
	{
		// breche gerade aktive Loop (while, do, for) durch Aufwerfen einer Exception ab,
		// diese Exception wird von den Loop-Behandlungsroutinen abgefangen und bearbeitet !
		throw minContinueException();
		return true;
	}
	return false;
}

#ifdef USEBIG
bool minContinueNode::GenerateCppCode( string & sCodeOut )
{
	sCodeOut += "continue;";
	return true;
}
#endif
