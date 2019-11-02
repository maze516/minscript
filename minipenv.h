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

#ifndef _MINIPENV_H
#define _MINIPENV_H

//*************************************************************************
#include "mstl_string.h"
#include "mstl_list.h"
#include "mstl_vector.h"

//*************************************************************************
#include "dllexport.h"
#include "minhandle.h"

//*************************************************************************
// Vorausdeklarationen
class minCallStackItem;
class minBlockNode;
class minFunctionDeclarationNode;
class minClassDeclarationNode;
class minCreatorInterface;
class minNativeFcnWrapperBaseAdapter;
class minInterpreterNode;
class NativeFcnWrapperBase;

//*************************************************************************

// +++ einige Konstanten fuer Runtime Fehler-Codes +++
const int ENV_ERROR_VAR_NOT_FOUND =			5001;
const int ENV_ERROR_CALLING_FCN =			5002;
const int ENV_ERROR_CALLING_FCN2 =			5003;
const int ENV_ERROR_REQ_L_VALUE =			5004;
const int ENV_ERROR_DIV_WITH_ZERO =			5005;
const int ENV_ERROR_FCN_NOT_FOUND =			5006;
const int ENV_ERROR_IN_EXPR	=				5007;
const int ENV_ERROR_BREAK_OR_CONTINUE_NOT_ALLOWED	= 5008;
const int ENV_UNEXPECTED_ERROR =			5009;
const int ENV_ERROR_NO_OBJECT =				5010;
const int ENV_ERROR_NO_ARRAYOBJECT =		5011;
const int ENV_ERROR_CLASS_NOT_FOUND =		5012;
const int ENV_ERROR_BAD_ARRAYINDEX =		5013;
const int ENV_ERROR_FCN_ALLREADY_DEFINED =	5014;
const int ENV_ERROR_CALLING_METHOD =		5015;
const int ENV_ERROR_CONSTRUCTOR_NOT_FOUND =	5016;
const int ENV_ERROR_DEREF_NULL_PTR =		5017;
const int ENV_ERROR_NO_PTR =				5018;
const int ENV_ERROR_CAN_NOT_CAST =			5019;
const int ENV_ERROR_NO_OBJ_PTR =			5020;
const int ENV_ERROR_NO_THIS =				5021;
const int ENV_ERROR_NO_L_VALUE =			5022;
const int ENV_ERROR_VAR_IS_CONST =			5023;
const int ENV_ERROR_IN_CONSTRUCTOR_INIT =	5024;
const int ENV_ERROR_BREAKPOINT =			5025;
const int ENV_ERROR_RETURN_NOT_ALLOWED =	5026;

#define g_sManglingSeparator		"%"			// sollte ein Zeichen sein, was NICHT in einem Klassen-Bezeichner stehen darf !
#define g_sManglingVoid				"%void"
#define g_sPointerSeparator			"*"
#define g_sReferenceSeparator		"&"

extern const char * g_sClassMethodSeparator;

extern const char * g_sFunctionCallStart;

const int c_iMaxBuffer =			256;		// Groesse fuer temporaerre String-Buffer

//*************************************************************************

const int UNKNOWN_ARRAY_SIZE =		-2;

//*************************************************************************

class MINDLLEXPORT minRuntimeException
{
public:
	minRuntimeException( const string & sMessage ) : m_sMessage( sMessage ) {}

	string GetInfoString() const	{ return m_sMessage; }

private:
	string	m_sMessage;
};

//*************************************************************************

typedef enum { None, Private, Protected, Public } InterpreterClassScope;

typedef enum { Unknown, 
			   Void, 
			   Bool, 
			   CharTT, 
			   Int, 
			   Double, 
			   String, 
			   Object, 
			   Function, 
			   Array, 
			   TemplateType }	InterpreterValueType;

// Zur Implementation von Vererbungshirachien
typedef list< minHandle<minCallStackItem> >	minBaseObjectList;

// ist es ein einfacher (eingebauter) Typ, d.h. KEIN Objekt !
bool IsSimpleType( InterpreterValueType aType );

//*************************************************************************
/** Klasse zur Raepraesentation eines Types einer Interpreter-Variable. */
class MINDLLEXPORT minInterpreterType
{
	friend class minInterpreterValue;

public:
	minInterpreterType( InterpreterValueType aType = Unknown, int nPointerLevel = 0, bool bIsReference = false, const string & sClassName = "", bool bIsConst = false )
		: m_aType( aType ), m_sClassName( sClassName ), m_nPointerLevel( nPointerLevel ), 
		  m_bIsReference( bIsReference ), m_bIsConst( bIsConst )
	{}

	bool operator==( const minInterpreterType & aOther ) const
	{
		return m_aType==aOther.m_aType && (m_aType==Object ? m_sClassName==aOther.m_sClassName : true) &&
			   m_nPointerLevel==aOther.m_nPointerLevel && m_bIsReference==aOther.m_bIsReference;				// Zeile neu seit 5.2.2000
	}
	bool operator!=( const minInterpreterType & aOther ) const	{ return !(*this==aOther); }
#if defined( _MSC_VER )
	// Dummy-Implementationen
	bool operator<( const minInterpreterType & aOther ) const	{ return m_sClassName < aOther.m_sClassName; }
	bool operator>( const minInterpreterType & aOther ) const	{ return m_sClassName > aOther.m_sClassName; }
#endif

	// liefert den Typ des Dereferenz-Operators
	minInterpreterType GetDereferenceType() const	{ return minInterpreterType( m_aType, m_nPointerLevel-1, m_bIsReference, m_sClassName ); }
	// liefert den Typ des Zeiger-Operators
	minInterpreterType GetPointerType() const		{ return minInterpreterType( m_aType, m_nPointerLevel+1, /*m_bIsReference*/false, m_sClassName ); }		// false neu seit 5.2.2000
	minInterpreterType GetPointerRefType() const	{ return minInterpreterType( m_aType, m_nPointerLevel+1, /*m_bIsReference*/true, m_sClassName ); }		// false neu seit 5.2.2000

	bool	IsValidType() const						{ return m_aType!=Unknown; }
	bool	IsConst() const							{ return m_bIsConst; }
	bool	IsString() const						{ return !IsPointer() && (m_aType==String); }
	bool	IsObject() const						{ return !IsPointer() && (m_aType==Object); }
	bool	IsArray() const							{ return !IsPointer() && (m_aType==Array); }
	bool	IsFunction() const						{ return m_aType==Function; }
	bool	IsPointer() const						{ return m_nPointerLevel>0; }
	bool	IsReference() const						{ return m_bIsReference; }

	int		GetPointerLevel() const					{ return m_nPointerLevel; }

	void	SetType( const InterpreterValueType & aNewType )	{ m_aType = aNewType; }

	InterpreterValueType	GetType() const			{ return m_aType; }
	string					GetTypeString() const;
	string					GetRealTypeString() const;	// Typ ohne Zeiger und Referenz
	string					GetManglingString() const;

	void SetConst( bool bConst )					{ m_bIsConst = bConst; }

	bool GenerateCppCode( string & sCodeOut ) const;

private:
	// +++ Hilfsmethoden +++
	string GetTypeStringHelper( bool bManglingName, bool bWithPtrRef ) const;

	// +++ Daten +++
	InterpreterValueType	m_aType;
	string					m_sClassName;
	int						m_nPointerLevel;
	bool					m_bIsReference;
	bool					m_bIsConst;			// neu seit 28.12.1999
};

//*************************************************************************
// liefert den maximalen Typ der beiden uebergebenen Typen
//minInterpreterType GetMaxType( minInterpreterType aLeftType, minInterpreterType aRightType );

//*************************************************************************
class minInterpreterValue;

// Hilfstyp fuer Implemetation von Datentypen
union minIpValueImplHelper
{
	// direkte Datentypen
	long long								m_lValue;
	double									m_dValue;
	string *								m_psValue;				// Eigentuemer
	minHandle<minCallStackItem>	*			m_phObjValue;			// Eigentuemer

	// Referenz
	minInterpreterValue *					m_pToReferenceValue;	// KEIN Eigentuemer
			
	// Zeiger
	minInterpreterValue *					m_pPointer;				// Eigentuemer
	
	minHandle<minFunctionDeclarationNode> *	m_phFunction;			// Eigentuemer
};

//*************************************************************************
/** Klasse zur Raepraesentation eines Wertes einer Interpreter-Variable. */
class MINDLLEXPORT minInterpreterValue
{
	friend class minCallStackItem;

	// zum Erzeugen einer zweiten Verpackung fuer ein Objekt (CallStackItem) !
	minInterpreterValue( minHandle<minCallStackItem> * phCallStackItem, minInterpreterType aType );

public:
	minInterpreterValue();
	minInterpreterValue( double dVal );
	minInterpreterValue( void * pVal );
#ifdef _with_bool
	minInterpreterValue( bool bVal );
#endif
	minInterpreterValue( int iVal );
	minInterpreterValue( long lVal );
	minInterpreterValue( long long lVal);
	minInterpreterValue( char cVal );
	minInterpreterValue( const string & sVal );
	minInterpreterValue( const char * sVal );
	minInterpreterValue( minInterpreterType aType );

	// zum Erzeugen von Zeigern (wird im new-Operator-Node und im Address-Node verwendet)!
	minInterpreterValue( minInterpreterType aType, minInterpreterValue * pIpVal/*, bool bIsOwner = true*/ );
	// zum Erzeugen von Funktionszeigern:
	minInterpreterValue( minHandle<minFunctionDeclarationNode> hFunction );

	// Kopieren des Interpreter-Values
	minInterpreterValue( const minInterpreterValue & aObj );

	// Kopieren des Interpreter-Values
	minInterpreterValue & operator=( const minInterpreterValue & aObj );

	~minInterpreterValue();

	// zum Erzeugen von Referenzen !
	void SetRef( minInterpreterValue * pIpVal, minInterpreterType aType );

	// zum Aendern des Types
	void SetType( minInterpreterType aNewType );

	// kopiert den Wert dieses InterpreterValues, d.h. 
	// ggf. wird der Wert die Referenz geliefert (falls
	// es eine Referenz ist!)
	minInterpreterValue CopyValue() const;

	// Dummy-Implementationen
	bool operator<( const minInterpreterValue & aOther ) const	{ return false; }
	bool operator>( const minInterpreterValue & aOther ) const	{ return false; }
	bool operator==( const minInterpreterValue & aOther ) const	{ return false; }
	bool operator!=( const minInterpreterValue & aOther ) const	{ return !(*this==aOther); }

/* Fuer Tests mit NativeFunction-Templates:

	string &	GetStringRef()
	{
		if( !m_aType.IsString() )
		{
			throw minRuntimeException( "Error: no valid string-reference !" );
		}
		if( IsReference() )
		{
			return m_aValue.m_pToReferenceValue->GetStringRef();
		}
		return *m_aValue.m_psValue;
	}
	double &	GetDoubleRef()
	{
		if( m_aType.GetType()!=Double )
		{
			throw minRuntimeException( "Error: no valid double-reference !" );
		}
		if( IsReference() )
		{
			return m_aValue.m_pToReferenceValue->GetDoubleRef();
		}
		return m_aValue.m_dValue;
	}
*/
    string		GetString( bool bDebugOutput = false )	const;
	double		GetDouble() const;
	bool		GetBool() const;
	long long	GetInt() const;
	char		GetChar() const;

	int			GetSizeofValue() const;

	int	GetPointerLevel() const			{ return m_aType.GetPointerLevel(); }

	// Addresse auf den Value liefern, fuer Address-Operator
	minInterpreterValue *				GetAddress();
	minInterpreterValue *				GetDereference();
	bool								DeletePointer();

	// TODO ggf. ist diese Methode nicht mehr notwendig (verwende Zuweisungsoperator = )
	bool								CastAndAssign( const minInterpreterValue & aObj );

// (30.12.1999) BUG unter Windows (linux ist ok !)!!! 
// Vorausdeklaration wird mit einem NICHT Aufruf des Destruktors quitiert !!! Daher nun als *
	minHandle<minCallStackItem> *		GetObjectCallStackItem();

	minHandle<minFunctionDeclarationNode> GetFunction();

	void SetCreator( const minCreatorInterface * pVariableCreator );

#ifdef _with_bool
	operator bool() const				{ return GetBool(); }
#endif
	operator int() const				{ return (int)GetInt(); }
	operator char()	const				{ return GetChar(); }
//	operator double &()					{ return GetDoubleRef(); }
	operator double() const				{ return GetDouble(); }
	operator const char *()				{ m_sTempForCast = GetString(); return m_sTempForCast.c_str(); }
	operator string() const				{ return GetString(); }
//	operator string &()					{ return GetStringRef(); }
	operator void *() const				{ return (void *)GetInt(); }

	bool	IsConst() const				{ return m_aType.IsConst(); }
	bool	IsNull() const;
	bool	IsString() const			{ return m_aType.IsString(); }
	bool	IsObject() const			{ return m_aType.IsObject(); }
	bool	IsArray() const				{ return m_aType.IsArray(); }
	bool	IsFunction() const			{ return m_aType.IsFunction(); }
	bool	IsPointer() const			{ return m_aType.IsPointer(); }
	bool	IsReference() const			{ return m_aType.IsReference(); }

	minInterpreterType &	GetInterpreterTypeRef()			{ return m_aType; }
	minInterpreterType		GetInterpreterType() const		{ return m_aType; }
	InterpreterValueType	GetType() const					{ return m_aType.GetType(); }
	string					GetTypeString() const			{ return m_aType.GetTypeString(); }
	
	bool	GenerateCppCode( string & sCodeOut )			{ return false; }	// TODO

	string	GetInfoString() const;

	void *	GetRepAddr() const;

private:
	// DIE Konversions-Methode !!!
    minInterpreterValue ConvertTo( const minInterpreterType & aToType, bool bDebugOutput = false ) const;

	void Assign( const minInterpreterValue & aObj );
	void DeleteDataIfOwner();

	// +++ Daten +++

	// *** welchen Type repraesentiert dieses Objekt ?
	minInterpreterType				m_aType;

	// das eigentliche Datum
	minIpValueImplHelper			m_aValue;

	//bool							m_bIsOwner;			// fuer this Behandlung, Zeiger ohne Eigentuemerschaft !

	// temp
	string							m_sTempForCast;		// nur fuer cast: const char * notwendig
};

typedef minInterpreterValue minInterpreterValue;

//*************************************************************************
//*************************************************************************

//*************************************************************************
// repraesentiert eine Variable im Interpreter
class MINDLLEXPORT minInterpreterVariable
{
public:
	minInterpreterVariable( const string & sName = "", minHandle<minInterpreterValue> hVal = 0, const string & sAliasName = "" )
		: m_sName( sName ), m_sAliasName( sAliasName ), m_hValue( hVal )
	{}

	// lesender Zugriff
	const string &					GetName() const			{ return m_sName; }
	const string &					GetAliasName() const	{ return m_sAliasName; }
	minHandle<minInterpreterValue>	GetValue()				{ return m_hValue; }

	int								GetSizeofValue() const	{ return m_hValue->GetSizeofValue(); }

	string							GetInfoString() const	{ return m_sName+"{"+m_hValue->GetTypeString()+"}="+m_hValue->GetInfoString(); }

	// diverse Operatoren
	bool operator==( const minInterpreterVariable & aItem ) const	{ return m_sName == aItem.m_sName || m_sAliasName == aItem.m_sName; }
	bool operator!=( const minInterpreterVariable & aItem ) const	{ return !((*this)==aItem); }
	bool operator<( const minInterpreterVariable & aItem ) const	{ return m_sName < aItem.m_sName; }
#ifdef _MSC_VER
	bool operator>( const minInterpreterVariable & aItem ) const	{ return m_sName > aItem.m_sName; }
#endif

private:

	// +++ Daten +++
	string							m_sName;
	string							m_sAliasName;	// zweiter Name, wird z.B. verwendet um auf Funktions-Argumente bei zuzugreifen
	minHandle<minInterpreterValue>	m_hValue;
};

//*************************************************************************
// repraesentiert einen Call-Stack Eintrag, enthaelt z.B. lokale Variablen, etc.
class MINDLLEXPORT minCallStackItem
{
	friend class minBlockNode;

public:
	typedef list<minInterpreterVariable>	VariableContainerT;

	minCallStackItem(const string & sItemName = "", bool bHidesObject = false, int nLineNumber = 0);
	minCallStackItem( const minCallStackItem & aOther );
	~minCallStackItem();

	const VariableContainerT & GetVariables() const { return m_aVariableContainer; }

	minCallStackItem & operator=( const minCallStackItem & aOther );

	// fuer Parent Beziehungen
	void AddBaseObject( minHandle<minCallStackItem> hBaseObject );

#ifdef _MSC_VER
	// diverse Operatoren
	bool operator==( const minCallStackItem & aItem ) const	{ return this == &aItem; }
	bool operator!=( const minCallStackItem & aItem ) const	{ return !((*this)==aItem); }
	bool operator<( const minCallStackItem & aItem ) const	{ return false; }
	bool operator>( const minCallStackItem & aItem ) const	{ return false; }
#endif

	const string & GetItemName() const				{ return m_sItemName; }

	const string & GetUserName() const				{ return m_sUserName; }
	void SetUserName( const string & sNewName )		{ m_sUserName = sNewName; }
	bool IsObject() const							{ return m_sUserName.length()>0; }	// wird verwendet in minFunctionCallNode::Execute()
	bool HidesObject() const						{ return m_bHidesObject; }

	bool ExistsVariable( const string & sName ) const;

	// liefert Handle auf den orginal Wert, d.h. L-Value
	minHandle<minInterpreterValue> GetValueForVariable( const string & sName );
	// hier koennen NUR orginal Daten abgelegt werden, der CallStack-Item wird Eigentuemer von diesen Daten !
	bool AddNewVariable( const string & sName, minHandle<minInterpreterValue> hValue, const string & sAliasName = "" );

	// liefert einen String zurueck, der Infos ueber den Stack-Eintrag enthaelt,
	// z.B. Name der Funktion und alle lokalen Variablen
	string	GetInfoString( bool bShowContent = false ) const;

	int		GetSizeofValue() const;

	// Methoden fuer die this-Behandlung (seit 4.2.2003),
	// falls nicht vorhanden wird this angelegt.
	minHandle<minInterpreterValue> GetThis();

	const minCreatorInterface *	GetCreator() const						{ return m_pVariableCreator; }
	void SetCreator( const minCreatorInterface * pVariableCreator )		{ m_pVariableCreator = pVariableCreator; }

	void SetCurrentLine( int nCurrentLineNo )							{ m_nCurrentLineNumber = nCurrentLineNo; }
	int  GetCurrentLine() const											{ return m_nCurrentLineNumber; }

	int  GetLine() const												{ return m_nLineNumber; }

	void DumpVariables( ostream & stream );

private:
	// +++ Hilfsmethoden +++
	void Assign( const minCallStackItem & aOther );

	// +++ Daten +++
	bool					m_bHidesObject;			// neu seit 17. 1.2003
	string					m_sItemName;
	string					m_sUserName;			// neu seit 19.12.1999
	int						m_nLineNumber;			// neu seit 15. 7.2015	line number of caller
	int						m_nCurrentLineNumber;	// neu seit 14.10.2016
	VariableContainerT		m_aVariableContainer;
	minBaseObjectList *		m_paBaseObjectList;		// neu seit 22. 1.2000
	minInterpreterValue *	m_pThisObj;				// neu seit  4. 2.2003

	// fuer Template-Behandlung
	const minCreatorInterface *	m_pVariableCreator;		// KEIN EIGENTUEMER, neu seit 14.2.2003
};

//*************************************************************************
class minInterpreterEnvironmentInterface
{
public:
	virtual bool AddNativeFunction( NativeFcnWrapperBase * pNativeFunc ) = 0;
};

//*************************************************************************
struct minBreakpointInfo
{
	int		iLineNo;
	string  sCondition;
	string  sFileName;
};

//*************************************************************************
class minDebuggerInfo
{
public:
	int		iLineNo;

	static minHandle<minDebuggerInfo> CreateDebuggerInfo( int iLineNo );
};

//*************************************************************************
// enthaelt z.B. lokale Variablen, Funktionen etc.
class MINDLLEXPORT minInterpreterEnvironment : public minInterpreterEnvironmentInterface
{
	typedef list< minHandle<minCallStackItem> >				CallStackContainerT;				
	typedef list< minHandle<minFunctionDeclarationNode> >	FunctionContainerT;	
	typedef list< minHandle<minClassDeclarationNode> >		ClassContainerT;

	typedef list< minBreakpointInfo >						BreakpointContainerT;

public:
	minInterpreterEnvironment();
	~minInterpreterEnvironment();

    // Flag to indicate comand line debugger
    void SetDbgMode( bool bDebug )		{ m_bDbg = bDebug; }
    bool IsDbgMode() const				{ return m_bDbg; }

	void SetDbgResetExecution(bool bReset) { m_bDbgResetExecution = bReset; }
	bool IsDbgResetExecution() const	{ return m_bDbgResetExecution; }

	// Flag to indicate an extended tracing
    void SetDebugMode( bool bDebug )	{ m_bDebug = bDebug; }
	bool IsDebugMode() const			{ return m_bDebug; }

	void SetSilentMode( bool bSilent )	{ m_bIsSilent = bSilent; }
	bool IsSilentMode() const			{ return m_bIsSilent; }

	void SetSourceCode( const string & sCode )	{ m_sSourceCode = sCode; }

	void SetLineCountOfAddedCode(int nLineCount) { m_nLineCountOfAddedCode = nLineCount; }
	
	// Groesse des aktuellen Call-Stacks liefern
	int GetCallStackSize() const				{ return (int)m_aCallStack.size(); }
	// den gerade aktuellen Call-Stack Eintrag liefern (Spize des Stacks)
	minHandle<minCallStackItem> GetActCallStackItem();
	// der unter dem gerade aktuellen Call-Stack Eintrag liegenden Eintrag liefern (Spize des Stacks - 1), fuer this-Implementation
	minHandle<minCallStackItem> GetActCallStackItemMinusOne();
	// Zugriff auf den Callstack via Level (1..n)
	minHandle<minCallStackItem> GetCallStackItemForLevel( int iLevel );
	// den uebergebenen Call-Stack Eintrag als oberstes Element ablegen
	bool PushCallStackItem( minHandle<minCallStackItem> aCallStackItem );
	// neuen Call-Stack Eintrag erzeugen
	bool PushCallStackItem( const string & sItemName, bool bHidesObject = false, int nLineNumber = 0 );
	// den obersten Call-Stack Eintrag vom Stack nehmen (und zerstoeren)
	bool PopCallStackItem();

	// liefert das letzte auf dem Stack liegende Objekt, falls
	// der Call-Stack nicht durch irgend einen Funktionsaufruf
	// unterbrochen ist:
	// Beispiel:
	//     __main():
	//     block
	//     Object{MyClass}:...
	//     MyClass_f():					// Methode, d.h. liefere Object
	//     f():							// Funktion, d.h. liefere 0
	//
	minHandle<minCallStackItem> GetLatestVisibleObjectOnStack( int iStackDelta = 0 ) const;

	// liefert den aktuellen Klassen-Skope fuer das letzte/oberste Objekt
	// auf dem Callstack. Z.B. kann auch die Basisklasse geliefert werden,
	// wenn eine Methode dieser Basisklasse gerade aufgerufen wird.
	/// Diese Funktionalitaet ist notwendig fuer die korrekte Implementierung
	// von virtuellen Methoden.
	// Beispiel:
	//     __main():
	//     block
	//     Object{DerivedClass}:...
	//     BaseClass_Dump():			// Methode aus der Basisklasse
	//     block
	// --> liefert "BaseClass" zurueck.
	string GetLatestClassScopeForLatestObject() const;

	// liefert Handle auf den orginal Wert, d.h. L-Value
	minHandle<minInterpreterValue> GetValueForVariable( const string & sName );
	// hier koennen NUR orginal Daten abgelegt werden, der CallStack-Item wird Eigentuemer von diesen Daten !
	bool AddNewVariable( const string & sName, minHandle<minInterpreterValue> hValue );

	// Methoden zum Verwalten von Funktionen,
	// ACHTUNG: das Environment wird NICHT Eigentuemer der uebergebenen Funktionen !!!
	int GetNoOfFunctions( const string & sName ) const;
	minHandle<minFunctionDeclarationNode> GetFunction( const string & sName ) const;
	minHandle<minFunctionDeclarationNode> GetFunctionForManglingName( const string & sName, const string & sManglingName ) const;
	bool AddFunction( minHandle<minFunctionDeclarationNode> hFunction );
	bool RemoveFunction( minHandle<minFunctionDeclarationNode> hFunction );
	void RemoveAllFunctions();
	void DumpAllFunctions( ostream & aStream ) const;
	void DumpAllFunctionPrototypes( ostream & aStream ) const;

	// Methoden zum Verwalten von User-Datentypen wie class und struct (neu seit 16.11.1999)
	minHandle<minClassDeclarationNode> GetClass( const string & sName ) const;
	bool AddClass( minHandle<minClassDeclarationNode> hClass );
	bool RemoveClass( minHandle<minClassDeclarationNode> hClass );
	void RemoveAllClasses();

	// fuer die Behandlung von templates
	minInterpreterType ResolveTypeFromString( const string & sTypeString, int iStackDelta = 0 ) const;
	bool CheckForTemplateType( minInterpreterType & aType ) const;

	// Methoden fuer die Fehlerbehandlung
	int		GetLastErrorCode() const			{ return m_nLastErrorCode; }
	string	GetLastErrorMsg() const				{ return m_sLastErrorMsg; }
    bool    HasError() const                    { return GetLastErrorCode()!=0; }
	void	SetError( int nErrorCode, const string & sMsg ) 
	{ 
		m_nLastErrorCode = nErrorCode; 
		m_sLastErrorMsg = sMsg;
	}
	// erzeugt eine Fehlerbehandlung/Exception --> z.B. Unterstuetzung von Registrierung von Callbacks !
    void	ProcessError( minInterpreterNode * pCurrentNode = 0 );

	string GetInfoString() const;

	// *** implement the interfaces ***
	virtual bool AddNativeFunction( NativeFcnWrapperBase * pNativeFunc );

    // Implement command line debugger
    bool ProcessDbg( minInterpreterNode * pCurrentNode );
    
	// zum Testen
	void Dump() const;

	void ResetDebuggerExecutionFlags();

private:
	bool IsAtBreakpoint( minInterpreterNode * pCurrentNode ) const;
	bool IsAtBreakpoint( int iLineNo ) const;
	list<int> GetBreakpointLines() const;
	vector<string> GetCallStackForDebugger( const string & sSourceCode, const CallStackContainerT & aCallStack, int iSelectedCallStackLevel ) const;
	minCallStackItem::VariableContainerT GetVairablesForDebugger( const CallStackContainerT & aCallStack, int iSelectedCallStackLevel ) const;
	pair<int, int> GetLineNoOfCallStackItem(const CallStackContainerT & aCallStack, int iSelectedCallStackLevel, int & iLevelDown, int & iLevelUp) const;

	CallStackContainerT		m_aCallStack;
	FunctionContainerT		m_aFunctionContainer;
	ClassContainerT			m_aClassContainer;
	BreakpointContainerT	m_aBreakpointContainer;
    // Variablen fuer die Fehlerbehandlung und Debugging
	int						m_nLineCountOfAddedCode;
    int                     m_nCurrentLineNo;
    int                     m_nCurrentCallStackLevel;
	int						m_nCurrentDebuggerCallStackLevel;
	int						m_nLastBreakpointLineNo;
	int						m_nLastErrorCode;
	string					m_sLastErrorMsg;
	string					m_sSourceCode;
	bool					m_bDebug;
    bool                    m_bDbg;
	bool					m_bDbgResetExecution;
    bool                    m_bRunDbg;
    bool                    m_bStepToNextLine;
    bool                    m_bStepIntoNextLine;
	bool					m_bStepOut;
    bool					m_bIsSilent;
};

typedef minInterpreterEnvironment minIpEnv;

//MINDLLEXPORT InterpreterValueType GetTypeFromString( const string & aTypeString );

//*************************************************************************
// ein Debug-Verwaltungs-Objekte auf sehr niedriger Abstraktionsebene:
MINDLLEXPORT bool IsDebugOn();
MINDLLEXPORT void SetDebugModus( bool bOn );
MINDLLEXPORT ostream & GetDebugStream();
MINDLLEXPORT const char * GetMethodSeparatorStrg();

//*************************************************************************

extern void DumpVersion(ostream & out);

#include <assert.h>

#ifdef _DEBUG
#define MEMORY_DBG( x )			x
#define MEMORY_DBG_OUT( x )		
#else
#define MEMORY_DBG( x )			
#define MEMORY_DBG_OUT( x )
#endif

MEMORY_DBG( MINDLLEXPORT int GetValueCount(); )
MEMORY_DBG( MINDLLEXPORT int GetItemCount(); )

#endif
