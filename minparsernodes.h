/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minparsernodes.h,v $
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

#ifndef _MINPARSERNODES_H
#define _MINPARSERNODES_H

//*************************************************************************
#include "mstl_string.h"
#include "mstl_list.h"
#include "mstl_vector.h"
#include "mstl_iostream.h"

//*************************************************************************
#include "dllexport.h"
#include "minhandle.h"
#include "minipenv.h"		// fuer: minInterpreterValue
#include "mintoken.h"
#include "mintokenizer.h"	// fuer: StringListT

//*************************************************************************
class minInterpreterEnvironment;
class minInterpreterNode;
class minVariableDeclarationNode;
class minClassDeclarationNode;
class minClassBlockNode;
class minTemplateNode;

//*************************************************************************
// +++ Operator-Levels, siehe C++, Stroustrup, S.129
const int HIGHEST_LEVEL			= 0;		// fuer buildin operators, z.B. __exists
const int UNARY_OPERATOR_LEVEL	= 1;
const int MULT_LEVEL			= 2;
const int DIV_LEVEL				= 2;
const int SUB_LEVEL				= 3;
const int ADD_LEVEL				= 3;
const int SHIFT_LEVEL			= 4;
const int COMPARE_LEVEL			= 5;
const int EQUAL_LEVEL			= 6;
const int BIN_AND_LEVEL			= 7;
//const int BIN_EXLUSIV_OR_LEVEL	= 8;
const int BIN_OR_LEVEL			= 9;
const int LOG_AND_LEVEL			= 10;
const int LOG_OR_LEVEL			= 11;
const int ASSIGN_LEVEL			= 13;
const int COMMA_LEVEL			= 16;

#define NODE_STRING				"Node="
#define SPACE_SHIFT				"  "

#define _CACHE_NOT_INIT			"!"

#define _VARIABLEDECLARATIONNODE	"VariableDeclarationNode"
#define _FUNCTIONDECLARATIONNODE	"FunctionDeclarationNode"
#define _VARIABLENODE				"VariableNode"
#define _EMPTYNODE					"EmptyNode"
#define _BLOCKNODE					"BlockNode"
#define _CLASSNODE					"ClassDeclarationNode"

//*************************************************************************

// Bemerkung: minInterpreterNode ist ein virtuelle Basistyp und kann daher nicht instanziert werden !
typedef list< minHandle<minInterpreterNode> >				minParserItemList;

typedef vector< minHandle<minVariableDeclarationNode> >		minVariableDeclarationList;

// Type zur Verwaltung von Vererbungsbeziehungen
//typedef list< const minClassDeclarationNode * >			minBaseClassList;
typedef list< string >										minBaseClassList;

//*************************************************************************

inline string MakeMethodName( const string & sClassName, const string & sMethodName )
{
	// wenn keine Klasse angegeben ist, dann ist die Methode
	// wohl doch keine Methode sondern eine Funktion !
	if( sClassName.length()==0 )
	{
		return sMethodName;
	}
	return sClassName + g_sClassMethodSeparator + sMethodName;
}

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
/** Mit Hilfe dieser Klasse werden Native-Functions dem Interpreter bekannt gemacht. */
class MINDLLEXPORT minNativeFcnInterface
{
public:
	virtual ~minNativeFcnInterface();

	virtual const string &						GetFunctionName() const = 0;
	virtual int									GetFunctionTyp() const = 0;

	// Aufruf-Operatoren fuer die verschiedenen Function-Typen (0..9)
	virtual minInterpreterValue operator()();
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, minInterpreterValue & aParam6 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, minInterpreterValue & aParam6, minInterpreterValue & aParam7 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, minInterpreterValue & aParam6, minInterpreterValue & aParam7, minInterpreterValue & aParam8 );
	virtual minInterpreterValue operator()( minInterpreterValue & aParam1, minInterpreterValue & aParam2, minInterpreterValue & aParam3, minInterpreterValue & aParam4, minInterpreterValue & aParam5, minInterpreterValue & aParam6, minInterpreterValue & aParam7, minInterpreterValue & aParam8, minInterpreterValue & aParam9 );

	virtual minInterpreterType					GetReturnType() const = 0;

	virtual const minVariableDeclarationList &	GetArgumentList() const = 0;
};

//*************************************************************************
class minCreatorInterface
{
public:
	virtual const StringListT &	GetAllTemplateTypes() const = 0;
};

//*************************************************************************
// allgemeine Schnittstelle fuer einen Knoten im Parser-Baum, dieser Knoten
// kann von einem Interpreter verwerndet werden um ein Programm auszufuehren
class MINDLLEXPORT minInterpreterNode
{
public:
	minInterpreterNode( const string & sTypeName );
	minInterpreterNode( const minInterpreterNode & aOther );
	virtual ~minInterpreterNode();		// WICHTIG: virtueller Destruktor !

	const string &	GetClassName() const						{ return m_sTypeName; }

	virtual string	GetInfo() const								{ return "?"; }	

	// zur Optimierung: benoetigt ein InterpreterNode ggf. einen lokalen Stack-Eintrag ?
	virtual bool NeedStackItem() const							{ return false; }
	// ist dieser InterpreterNode ggf. ein Operator ?
	virtual bool IsOperator() const								{ return false; }
	// liefert die Wertigkeit des Operators
	virtual int  GetOperatorLevel() const						{ return -1; }

	virtual bool Execute( int nAccessModus, /*0==Value, 1==L-Value, 2==Testen_fuer_L-Value(keine Fehlermeldung erzeugen)*/
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut )	{ return false; } )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	static long & GetCountRef()									{ return m_nCount; }

private:
	string		m_sTypeName;
	// zum Debuggen
	static long	m_nCount;
};

//*************************************************************************
// Hilfsklasse: verwaltet eine Verzweigung (linker und rechter Knoten)
class minForkNode : public minInterpreterNode
{
protected:
	// Bem.; Uebernimmt Eigentuemerschaft an uebergebenen Zeigern !
	minForkNode( minInterpreterNode * pLeftNode, minInterpreterNode * pRightNode ) 
		: minInterpreterNode( "ForkNode" ), 
		  m_pLeftNode( pLeftNode ), 
		  m_pRightNode( pRightNode )
	{}
	virtual ~minForkNode();

	// +++ Daten +++
	minInterpreterNode *	m_pLeftNode;		// ist Eigentuemer !
	minInterpreterNode *	m_pRightNode;		// ist Eigentuemer !
};

//*************************************************************************
class minEmptyNode : public minInterpreterNode
{
public:
	minEmptyNode() : minInterpreterNode( _EMPTYNODE ) {}

	virtual bool Execute( int nAccessModus, 
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv )	{ return true; }
	SMALL( virtual bool GenerateCppCode( string & sCodeOut )	{ sCodeOut = ";"; return true; } )
};

//*************************************************************************
class minCommentNode : public minInterpreterNode
{
public:
	minCommentNode( const string & sCommentStrg ) 
		: minInterpreterNode( "CommentNode" ), 
		  m_sCommentStrg( sCommentStrg ) 
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv )	{ return true; }
	SMALL( virtual bool GenerateCppCode( string & sCodeOut )	{ sCodeOut = m_sCommentStrg; return true; } )

private:
	string					m_sCommentStrg;
};

//*************************************************************************
class minConstantNode : public minInterpreterNode
{
public:
	minConstantNode( const minInterpreterValue & aValue ) 
		: minInterpreterNode( "ConstantNode" ), 
		  m_aValue( aValue ) 
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

private:
	minInterpreterValue		m_aValue;
};

//*************************************************************************
class minVariableNode : public minInterpreterNode
{
public:
	minVariableNode( const string & sName, minInterpreterType aType = Unknown )
		: minInterpreterNode( _VARIABLENODE ), 
		  m_sName( sName )
	{}

	virtual string	GetInfo() const								{ return m_sName; }	

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

private:
	string					m_sName;
};

//*************************************************************************
// Deklaration einer Variable, z.B. int i = 0; float x,y;
class MINDLLEXPORT minVariableDeclarationNode : public minInterpreterNode, public minCreatorInterface
{
public:
	minVariableDeclarationNode( const string & sName, minInterpreterType aType, int nArraySize,
								minInterpreterNode * pInitArrayExpression,	
								minInterpreterNode * pInitExpression /*= 0*/,
								minInterpreterNode * pConstructorCall /*= 0*/,
								const StringListT & aTemplateTypes /*= StringListT()*/ );
	virtual ~minVariableDeclarationNode();

	virtual string	GetInfo() const								{ return "name="+m_sName; }	

	virtual bool NeedStackItem() const							{ return true; }

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	const string & GetName() const								{ return m_sName; }
	const minInterpreterType & GetType() const					{ return m_aType; }

	string GetManglingTypeName() const							{ return m_aType.GetManglingString(); }

	bool IsReference() const									{ return m_aType.IsReference(); }
	bool IsConst() const										{ return m_aType.IsConst(); }
	bool IsValidMemberDeclaration() const;
	void SetClassScope( InterpreterClassScope aNewScope )		{ m_aClassScope = aNewScope; }

	SMALL( bool GenerateCppCode( bool bIsFirstVar, string & sCodeOut ); )

	// Hilfsmethode fuer den Parser
	void SetNextDeclaration( minVariableDeclarationNode *pNextVarDecl )	{ m_pNextVarDecl = pNextVarDecl; }

	virtual const StringListT &	GetAllTemplateTypes() const		{ return m_aTemplateTypes; }

private:
	bool IsArray() const										{ return m_nArraySize!=-1; }

	string							m_sName;
	minInterpreterType				m_aType;			// neu seit 26.12.1999
	int								m_nArraySize;
	InterpreterClassScope			m_aClassScope;
	minInterpreterNode *			m_pInitArrayExpression;	// ist Eigentuemer
	minInterpreterNode *			m_pInitExpression;		// ist Eigentuemer
	minInterpreterNode *			m_pConstructorCall;		// ist Eigentuemer, neu seit 23.12.1999
	minVariableDeclarationNode *	m_pNextVarDecl;			// ist Eigentuemer ! Fuer Konstrukte wie: int i,j=5,k;
	StringListT						m_aTemplateTypes;	// neu seit 14.2.2003
};

//*************************************************************************
// Klassen-Deklaration
class minClassDeclarationNode : public minInterpreterNode
{
public:
	minClassDeclarationNode( const string & sName, minClassBlockNode * pCode, minBaseClassList aBaseClassList, minTemplateNode * pTemplate = 0 )
		: minInterpreterNode( _CLASSNODE ), 
		  m_sName( sName ), 
		  m_pCode( pCode ), 
		  m_aBaseClassList( aBaseClassList ), 
		  m_pEnv( 0 ),
		  m_pTemplate( pTemplate )
	{}
	virtual ~minClassDeclarationNode();

	const string &	GetName() const								{ return m_sName; }

	virtual string	GetInfo() const								{ return "name="+m_sName; }	

	bool InitObject( minInterpreterEnvironment & aEnv );
	bool ExecuteDestructor( minInterpreterEnvironment & aEnv );
	bool IsDefined() const										{ return m_pCode!=0; }
	minClassBlockNode *			GetClassBlock()					{ return m_pCode; }
	const minBaseClassList &	GetAllBaseClasses() const		{ return m_aBaseClassList; }

	void SetTemplate( minTemplateNode * pTemplate )				{ m_pTemplate = pTemplate; }
	const minTemplateNode * GetTemplate() const					{ return m_pTemplate; }
	minInterpreterType GetTemplateType( const string & sTypeName, const StringListT & aRealTypes, const minInterpreterEnvironment & aEnv, int iStackDelta ) const;

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

private:
	string						m_sName;
	minClassBlockNode *			m_pCode;			// ist Eigentuemer, hier wird sich die ganze Klassen-Deklaration gemerkt !
	minBaseClassList			m_aBaseClassList;	// neu seit 22.1.2000
	// Referenz auf das Environment an dem die Klasse sich registriert 
	minInterpreterEnvironment * m_pEnv;				// KEIN Eigentuemer
	// Referenz auf den Template-Node, falls diese Klasse ein Template ist 
	minTemplateNode *			m_pTemplate;		// KEIN Eigentuemer
};

//*************************************************************************
class minClassBlockNode : public minInterpreterNode
{
public:
	minClassBlockNode( const minParserItemList & aMethodList, const minParserItemList & aVariableList, const minParserItemList & aConstructorList, minHandle<minInterpreterNode> hDestructorNode );
	virtual ~minClassBlockNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	bool	InitMemberVariables( minInterpreterEnvironment & aEnv );
	bool	RegisterMethods( minInterpreterEnvironment & aEnv );
	bool	UnRegisterMethods( minInterpreterEnvironment & aEnv );
	bool	RegisterConstructorsDestructors( minInterpreterEnvironment & aEnv );
	bool	UnRegisterConstructorsDestructors( minInterpreterEnvironment & aEnv );

	// ACHTUNG: nur fuer die stub-file Erzeugung gedacht !
	SMALL( const minParserItemList	&			GetMethods() const		{ return m_aMethodNodeContainer; } )
	SMALL( const minParserItemList	&			GetConstructors() const	{ return m_aConstructorNodeContainer; } )
	SMALL( const minHandle<minInterpreterNode>	GetDestructor() const	{ return m_hDestructorNode; } )

private:
	minParserItemList				m_aMethodNodeContainer;
	minParserItemList				m_aMemberVariableNodeContainer;
	minParserItemList				m_aConstructorNodeContainer;
	minHandle<minInterpreterNode>	m_hDestructorNode;				// es darf nur einen Destruktor fuer die Klasse geben !
};

//*************************************************************************
class minTemplateNode : public minInterpreterNode
{
public:
	minTemplateNode( const StringListT & aTemplateTypes, minInterpreterNode * pClass );
	virtual ~minTemplateNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
//	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	minInterpreterType GetTemplateType( const string & sTypeName, const StringListT & aRealTypes, const minInterpreterEnvironment & aEnv, int iStackDelta ) const;

private:
	StringListT						m_aTemplateTypes;
	minInterpreterNode *			m_pClass;
};

//*************************************************************************
// Basisklasse fuer Funktions-Deklaration
class MINDLLEXPORT minFunctionDeclarationNode : public minInterpreterNode
{
protected:
	minFunctionDeclarationNode( 
			const string & sName, 
			bool bIsConstant, 
			bool bIsVirtual,
			minInterpreterType aReturnType, 
			const minVariableDeclarationList & aArgumentDeclarationList )
		: minInterpreterNode( _FUNCTIONDECLARATIONNODE ),
		  m_sName( sName ), 
		  m_bIsConstant( bIsConstant ),
		  m_bIsVirtual( bIsVirtual ),
		  m_aReturnType( aReturnType ), 
		  m_aClassScope( None ),
		  m_aArgumentDeclarationList( aArgumentDeclarationList )
	{
		UpdateArgumentList();
	}

public:
	virtual ~minFunctionDeclarationNode();

	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	virtual string	GetInfo() const										{ return "name="+m_sName+"()"; }	

	// +++ eigene Schnittstelle +++

	// den eigenen Code der Funktion ausfuehren
	virtual bool DoExecuteFcnBlock( minInterpreterValue & aReturnValOut, 
									minInterpreterEnvironment & aEnv )	{ return false; }
	virtual minParserItemList GetInitList() const;

	// den Namen der Funktion liefern
	const string &					GetName() const						{ return m_sName; }
	minInterpreterType				GetReturnType() const				{ return m_aReturnType; }
	string							GetPrototypeString() const;
	InterpreterClassScope			GetClassScope() const				{ return m_aClassScope; }

	bool							IsMethod() const					{ return m_aClassScope!=None; }
	bool							IsConstant() const					{ return m_bIsConstant; }
	bool							IsVirtual() const					{ return m_bIsVirtual; }
	// durch Aufruf dieser Methode wird diese Funktion zu einer Methode !
	void							SetNewMethodName( const string & sNewName, InterpreterClassScope aClassScope );

	string							GetManglingName() const;

	// lesender Zugriff !
	const minVariableDeclarationList &	GetArgumentsList() const		{ return m_aArgumentDeclarationList; }

	// ACHTUNG: nur fuer die stub-file Erzeugung gedacht !
	SMALL( void SetArgumentsList( minVariableDeclarationList & aNewArgsList )	{ m_aArgumentDeclarationList = aNewArgsList; } )
	SMALL( void SetReturnType( minInterpreterType aNewRetType )					{ m_aReturnType = aNewRetType; } )

protected:
	void							UpdateArgumentList();

	minInterpreterType				m_aReturnType;
	InterpreterClassScope			m_aClassScope;				// neu seit 28.12.1999
	minVariableDeclarationList		m_aArgumentDeclarationList;	// ist auch Eigentuemer an den Zeigern --> Automatisch erledigt durch Handle-Klasse !

private:
	string							m_sName;
	bool							m_bIsConstant;				// neu seit 28.12.1999
	bool							m_bIsVirtual;				// neu seit 30.1.2003
};

//*************************************************************************
// Deklaration einer im Script definierten Funktion
class minInterpreterFunctionDeclarationNode : public minFunctionDeclarationNode
{
public:
	minInterpreterFunctionDeclarationNode( 
			const string & sName, 
			bool bIsConstant, 
			bool bIsVirtual,
			minInterpreterType aReturnType, 
			const minVariableDeclarationList & aArgumentDeclarationList, 
			minInterpreterNode * pCode, 
			const minParserItemList & aConstructorInitList )
		: minFunctionDeclarationNode( sName, bIsConstant, bIsVirtual, aReturnType, aArgumentDeclarationList ), 
		  m_pCode( pCode ), 
		  m_aInitList( aConstructorInitList ),
		  m_pEnv( 0 )
	{}
	virtual ~minInterpreterFunctionDeclarationNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	// +++ ueberladen der Methode aus minFunctionDeclarationNode +++
	virtual bool DoExecuteFcnBlock( minInterpreterValue & aReturnValOut, 
									minInterpreterEnvironment & aEnv );
	virtual minParserItemList GetInitList() const;

private:
	minInterpreterNode *			m_pCode;		// ist Eigentuemer
	minParserItemList 				m_aInitList;	// neu seit 22.1.2000 (Implementation fuer Konstruktor-Initialisierungen, ggf. Optimierung ueber OnDemand Zeiger !)
	// Referenz auf das Environment an dem die Funktion sich registriert 
	minInterpreterEnvironment * 	m_pEnv;			// KEIN Eigentuemer
};

//*************************************************************************
// Deklaration einer native Funktion (z.B. fuer Runtime-Funktionen)
class MINDLLEXPORT minNativeFunctionDeclarationNode : public minFunctionDeclarationNode
{
public:
	minNativeFunctionDeclarationNode( minNativeFcnInterface * pFcn )
		: minFunctionDeclarationNode( pFcn->GetFunctionName(), /*bIsConst*/false, /*bIsVirtual*/false, pFcn->GetReturnType(), 
  		    /*ACHTUNG: hier wird die Eigentuemerschaft an dem Argument-Zeiger uebernommen !*/ pFcn->GetArgumentList() ), 
		  m_pFcn( pFcn )
	{}
	virtual ~minNativeFunctionDeclarationNode();

	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	// +++ ueberladen der Methode aus minFunctionDeclarationNode +++
	virtual bool DoExecuteFcnBlock( minInterpreterValue & aReturnValOut, 
									minInterpreterEnvironment & aEnv );

private:
	minNativeFcnInterface *		m_pFcn;			// ist Eigentuemer !
};

//*************************************************************************
// Funktionsaufruf, Konstruktoraufruf
class minFunctionCallNode : public minInterpreterNode
{
public:
	minFunctionCallNode( const string & sName, const minParserItemList & aArgumentExpressionList, minInterpreterNode * pFunctionExpression = 0 )
		: minInterpreterNode( "FunctionCallNode" ),
		  m_sName( sName ), 
		  m_sManglingNameCache( _CACHE_NOT_INIT ),
		  m_aArgumentExpressionList( aArgumentExpressionList ), 
		  m_pFunctionExpression( pFunctionExpression )
	{}
	virtual ~minFunctionCallNode();

	virtual string	GetInfo() const								{ return "call="+m_sName+"()"; }	

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	const string &	GetName() const								{ return m_sName; }

private:
	bool	IsManglingNameCacheOk() const						{ return m_sManglingNameCache!=string( _CACHE_NOT_INIT ); }
	//bool	InitManglingNameCache( minInterpreterEnvironment & aEnv );

	// +++ Daten +++
	string						m_sName;
	string						m_sManglingNameCache;			// neu seit 27.12.1999
	minParserItemList			m_aArgumentExpressionList;
	minInterpreterNode *		m_pFunctionExpression;			// ist Eigentuemer, neu seit 4.1.2000
};

//*************************************************************************
// Klasse fuer binaere Operatoren
class minOperatorNode : public minForkNode
{
protected:
	minOperatorNode( const string & sOperator, int nLevel, minInterpreterNode * pLeftNode, minInterpreterNode * pRightNode ) 
		: minForkNode( pLeftNode, pRightNode ), 
		  m_nOperatorLevel( nLevel ), 
		  m_sOperator( sOperator )
	{}

public:
	virtual string	GetInfo() const								{ return "op:"+m_sOperator; }	

	virtual bool IsOperator() const								{ return true; }
	virtual int  GetOperatorLevel() const						{ return m_nOperatorLevel; }

	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	// +++ neue, eigene Schnittstelle: +++
	void SetLeftNode( minInterpreterNode * pLeft )				{ m_pLeftNode = pLeft; }
	void SetRightNode( minInterpreterNode * pRight )			{ m_pRightNode = pRight; }
	minInterpreterNode * GetLeftNode() const					{ return m_pLeftNode; }
	minInterpreterNode * GetRightNode() const					{ return m_pRightNode; }

private:
	int						m_nOperatorLevel;
	string					m_sOperator;
};

//*************************************************************************
class minThisNode : public minInterpreterNode
{
public:
	minThisNode()
		: minInterpreterNode( "ThisNode" )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minExistsOperatorNode : public minOperatorNode
{
public:
	minExistsOperatorNode( const string & sOperator, minInterpreterNode * pNode = 0 ) 
		: minOperatorNode( sOperator, HIGHEST_LEVEL, 0, pNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
#ifdef USEBIG
class minTypeofOperatorNode : public minOperatorNode
{
public:
	minTypeofOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, HIGHEST_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minDereferenceExistsOperatorNode : public minOperatorNode
{
public:
	minDereferenceExistsOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, HIGHEST_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minDebugHaltNode : public minOperatorNode
{
public:
	minDebugHaltNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};
#endif

//*************************************************************************
class minNewOperatorNode : public minOperatorNode, public minCreatorInterface
{
public:
	minNewOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 ), 
		  m_pConstructorCall( 0 )
	{}
	virtual ~minNewOperatorNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );

	void SetType( minInterpreterType aType )							{ m_aType = aType; }
	void SetConstructorCall( minInterpreterNode * pConstructorCall )	{ m_pConstructorCall = pConstructorCall; }

	void SetAllTemplateTypes( const StringListT & aTemplateTypes )		{ m_aTemplateTypes = aTemplateTypes; }

	virtual const StringListT &	GetAllTemplateTypes() const				{ return m_aTemplateTypes; }

private:
	minInterpreterType		m_aType;
	minInterpreterNode *	m_pConstructorCall;
	StringListT				m_aTemplateTypes;
};

//*************************************************************************
class minDeleteOperatorNode : public minOperatorNode
{
public:
	minDeleteOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
// Bem.: dies ist ein post Unaray Operator:  pObj->   <==>  *pObj
class minPointerDereferenceOperatorNode : public minOperatorNode
{
public:
	minPointerDereferenceOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, HIGHEST_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minDereferenceOperatorNode : public minOperatorNode
{
public:
	minDereferenceOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minAddressOperatorNode : public minOperatorNode
{
public:
	minAddressOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minObjectElementNode : public minOperatorNode
{
public:
	minObjectElementNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, HIGHEST_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minArrayElementNode : public minOperatorNode
{
public:
	minArrayElementNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, HIGHEST_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minAssignOperatorNode : public minOperatorNode
{
public:
	// Bem.; Uebernimmt Eigentuemerschaft an uebergebenen Zeigern !
	minAssignOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, ASSIGN_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minIncOperatorNode : public minOperatorNode
{
public:
	minIncOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minDecOperatorNode : public minOperatorNode
{
public:
	minDecOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minNotOperatorNode : public minOperatorNode
{
public:
	minNotOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minInvertOperatorNode : public minOperatorNode
{
public:
	minInvertOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator, UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minCommaOperatorNode : public minOperatorNode
{
public:
	minCommaOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, COMMA_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minAddOperatorNode : public minOperatorNode
{
public:
	minAddOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, ADD_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minPlusOperatorNode : public minOperatorNode
{
public:
	minPlusOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator+string("sign"), UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minSubOperatorNode : public minOperatorNode
{
public:
	minSubOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, SUB_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minMinusOperatorNode : public minOperatorNode
{
public:
	minMinusOperatorNode( const string & sOperator ) 
		: minOperatorNode( sOperator+string("sign"), UNARY_OPERATOR_LEVEL, 0, 0 )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minMultOperatorNode : public minOperatorNode
{
public:
	minMultOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, MULT_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minDivOperatorNode : public minOperatorNode
{
public:
	minDivOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, DIV_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minModOperatorNode : public minOperatorNode
{
public:
	minModOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, DIV_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minShiftOperatorNode : public minOperatorNode
{
public:
	minShiftOperatorNode( const string & sOperator, bool bLeftShift, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, SHIFT_LEVEL, pLeftNode, pRightNode ),
		  m_bLeftShift( bLeftShift )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );

private:
	bool	m_bLeftShift;
};

//*************************************************************************
class minLessOperatorNode : public minOperatorNode
{
public:
	minLessOperatorNode( const string & sOperator, bool bLessEqual, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, COMPARE_LEVEL, pLeftNode, pRightNode ),
		  m_bLessEqual( bLessEqual )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );

private:
	bool	m_bLessEqual;
};

//*************************************************************************
class minMoreOperatorNode : public minOperatorNode
{
public:
	minMoreOperatorNode( const string & sOperator, bool bMoreEqual, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, COMPARE_LEVEL, pLeftNode, pRightNode ),
		  m_bMoreEqual( bMoreEqual )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );

private:
	bool	m_bMoreEqual;
};

//*************************************************************************
class minEqualOperatorNode : public minOperatorNode
{
public:
	minEqualOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, EQUAL_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minNotEqualOperatorNode : public minOperatorNode
{
public:
	minNotEqualOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, EQUAL_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minLogAndOperatorNode : public minOperatorNode
{
public:
	minLogAndOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, LOG_AND_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minLogOrOperatorNode : public minOperatorNode
{
public:
	minLogOrOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, LOG_OR_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minBinAndOperatorNode : public minOperatorNode
{
public:
	minBinAndOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, BIN_AND_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minBinOrOperatorNode : public minOperatorNode
{
public:
	minBinOrOperatorNode( const string & sOperator, minInterpreterNode * pLeftNode = 0, minInterpreterNode * pRightNode = 0 ) 
		: minOperatorNode( sOperator, BIN_OR_LEVEL, pLeftNode, pRightNode )
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
};

//*************************************************************************
class minParenthisNode : public minInterpreterNode
{
public:
	minParenthisNode( minInterpreterNode * pNode ) 
		: minInterpreterNode( "ParenthisNode" ), 
		  m_pNode( pNode ) 
	{}
	virtual ~minParenthisNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

private:
	minInterpreterNode * 	m_pNode;		// ist Eigentuemer
};

//*************************************************************************
class minBlockNode : public minInterpreterNode
{
public:
	minBlockNode( const minParserItemList & aNodeList ) 
		: minInterpreterNode( _BLOCKNODE ), 
		  m_aNodeContainer( aNodeList )
	{}
	virtual ~minBlockNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	// eigenes Interface:
	const minParserItemList & GetAllNodes() const;

private:
	bool NeedElementsStackItem() const;
	bool CallDestructors( minInterpreterEnvironment & aEnv );

	minParserItemList		m_aNodeContainer;
};

//*************************************************************************
class minTypedefNode : public minInterpreterNode
{
public:
	minTypedefNode( const minInterpreterType & aOldType, const string & sNewTokenString, minInterpreterNode * pClassDeclarationNode = 0 )
		: minInterpreterNode( "TypedefNode" ), 
		  m_aOldType( aOldType ), 
		  m_sNewTokenString( sNewTokenString ), 
		  m_pClassDeclarationNode( pClassDeclarationNode )
	{}
	virtual ~minTypedefNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )

private:
	minInterpreterType		m_aOldType;
	string					m_sNewTokenString;
	minInterpreterNode *	m_pClassDeclarationNode;	// ist Eigentuemer 
};

//*************************************************************************
class minCaseLabelNode : public minInterpreterNode
{
public:
	minCaseLabelNode( minInterpreterNode * pConstExpressionNode, const minParserItemList & aStatementList )
		: minInterpreterNode( "CaseNode" ), 
		  m_pConstExpressionNode( pConstExpressionNode ),
		  m_aStatementList( aStatementList )
	{}
	virtual ~minCaseLabelNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

	int		GetConstantValue( minInterpreterEnvironment & aEnv ) const;
	bool	IsDefaultLabel() const;

private:
	minInterpreterNode *	m_pConstExpressionNode;	// ist Eigentuemer
	minParserItemList		m_aStatementList;
};

//*************************************************************************
class minSwitchNode : public minInterpreterNode
{
public:
	minSwitchNode( minInterpreterNode * pExpressionNode, const minParserItemList & aCaseLabelList )
		: minInterpreterNode( "SwitchNode" ), 
		  m_pExpressionNode( pExpressionNode ),
		  m_aCaseLabelList( aCaseLabelList )
	{}
	virtual ~minSwitchNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

private:
	minInterpreterNode * 	m_pExpressionNode;	// ist Eigentuemer
	minParserItemList		m_aCaseLabelList;
};

//*************************************************************************
class minWhileNode : public minInterpreterNode
{
public:
	minWhileNode( minInterpreterNode * pExpressionNode, minInterpreterNode * pStatementNode )
		: minInterpreterNode( "WhileNode" ), 
		  m_pExpressionNode( pExpressionNode ), 
		  m_pStatementNode( pStatementNode ) 
	{}
	virtual ~minWhileNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
	SMALL( virtual bool Dump( ostream & aStream, const string & sSpace = "" ) const; )

private:
	minInterpreterNode * 	m_pExpressionNode;	// ist Eigentuemer
	minInterpreterNode *	m_pStatementNode;	// ist Eigentuemer
};

//*************************************************************************
class minDoNode : public minInterpreterNode
{
public:
	minDoNode( minInterpreterNode * pExpressionNode, minInterpreterNode * pStatementNode )
		: minInterpreterNode( "DoNode" ),
		  m_pExpressionNode( pExpressionNode ), 
		  m_pStatementNode( pStatementNode ) 
	{}
	virtual ~minDoNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )

private:
	minInterpreterNode * 	m_pExpressionNode;	// ist Eigentuemer
	minInterpreterNode *	m_pStatementNode;	// ist Eigentuemer
};

//*************************************************************************
class minForNode : public minInterpreterNode
{
public:
	minForNode( minInterpreterNode * pInitExpressionNode, 
			    minInterpreterNode * pCheckExpressionNode, 
				minInterpreterNode * pLoopExpressionNode, 
				minInterpreterNode * pStatementNode )
		: minInterpreterNode( "ForNode" ),
		  m_pInitExpressionNode( pInitExpressionNode ), 
		  m_pCheckExpressionNode( pCheckExpressionNode ), 
		  m_pLoopExpressionNode( pLoopExpressionNode ), 
		  m_pStatementNode( pStatementNode )
	{}
	virtual ~minForNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )

private:
	minInterpreterNode * 	m_pInitExpressionNode;	// ist Eigentuemer
	minInterpreterNode * 	m_pCheckExpressionNode;	// ist Eigentuemer
	minInterpreterNode * 	m_pLoopExpressionNode;	// ist Eigentuemer
	minInterpreterNode *	m_pStatementNode;		// ist Eigentuemer
};

//*************************************************************************
class minIfNode : public minInterpreterNode
{
public:
	minIfNode( minInterpreterNode * pExpressionNode, 
			   minInterpreterNode * pThenStatementNode, 
			   minInterpreterNode * pElseStatementNode )
		: minInterpreterNode( "IfNode" ),
		  m_pExpressionNode( pExpressionNode ), 
		  m_pThenStatementNode( pThenStatementNode ), 
		  m_pElseStatementNode( pElseStatementNode )  
	{}
	virtual ~minIfNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )

private:
	minInterpreterNode * 	m_pExpressionNode;		// ist Eigentuemer
	minInterpreterNode *	m_pThenStatementNode;	// ist Eigentuemer
	minInterpreterNode *	m_pElseStatementNode;	// ist Eigentuemer
};

//*************************************************************************
class minSizeofNode : public minInterpreterNode
{
public:
	minSizeofNode( minInterpreterNode *	pExpressionNode )
		: minInterpreterNode( "SizeofNode" ), 
		  m_pExpressionNode( pExpressionNode )
	{}
	virtual ~minSizeofNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )

private:
	minInterpreterNode *	m_pExpressionNode;		// ist Eigentuemer
};

//*************************************************************************
class minReturnNode : public minInterpreterNode
{
public:
	minReturnNode( minInterpreterNode *	pExpressionNode )
		: minInterpreterNode( "ReturnNode" ), 
		  m_pExpressionNode( pExpressionNode )
	{}
	virtual ~minReturnNode();

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )

private:
	minInterpreterNode *	m_pExpressionNode;		// ist Eigentuemer
};

//*************************************************************************
class minBreakNode : public minInterpreterNode
{
public:
	minBreakNode() 
		: minInterpreterNode( "BreakNode" ) 
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut, 
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
};

//*************************************************************************
class minContinueNode : public minInterpreterNode
{
public:
	minContinueNode() 
		: minInterpreterNode( "ContinueNode" ) 
	{}

	virtual bool Execute( int nAccessModus,
						  minInterpreterValue & aReturnValOut,
						  minInterpreterEnvironment & aEnv );
	SMALL( virtual bool GenerateCppCode( string & sCodeOut ); )
};

//*************************************************************************

inline bool IsGetLValue( int nAccessModus )
{
	if( nAccessModus==1 || nAccessModus==2 )
	{
		return true;
	}
	return false;
}

inline bool CheckLValueNode( int nAccessModus, minInterpreterEnvironment & aEnv )
{
	if( IsGetLValue( nAccessModus ) )
	{
		// nur bei Modus 1 auch eine Fehlermeldung erzeugen...
		if( nAccessModus==1 )
		{
			aEnv.SetError( ENV_ERROR_NO_L_VALUE, "const is no l-value" );
			aEnv.ProcessError();
		}
		return false;
	}
	return true;
}

//*************************************************************************

// +++ einige Hilfsklassen +++

// Basisklasse zur Behandlung von Implementations-Exception (break, continue und return)
class minImplException
{
public:
	virtual ~minImplException()	{}
};

// Hilfs-Klasse zur Implementierung der return-Anweisung
class minReturnException : public minImplException
{
public:
	minReturnException( const minInterpreterValue & aReturnValue )
		: m_aReturnValue( aReturnValue )
	{}

	const minInterpreterValue &	GetReturnValue() const	{ return m_aReturnValue; }

private:
	minInterpreterValue			m_aReturnValue;
};

// Hilfs-Klasse zur Implementierung der continue-Anweisung
class minContinueException : public minImplException
{
public:
};

// Hilfs-Klasse zur Implementierung der break-Anweisung
class minBreakException : public minImplException
{
public:
};

//*************************************************************************

typedef list<minInterpreterValue> _InterpreterValueContainer;


//*************************************************************************

// Vorausdeklaration (ehemals inline)
static string FindClassForMethod( const minInterpreterEnvironment & aEnv, const string & sClassScope, const string & sClassName, const string & sMethodName, bool bIsSearchInBaseClass, bool bTestVirtual );
static bool ExecuteConstructorHelper( const string & sClassName, minInterpreterValue & aReturnValOut, minInterpreterNode * pConstructorCall, minInterpreterEnvironment & aEnv );
static void ExecuteDestructorHelper( minInterpreterValue & aVar, minInterpreterEnvironment & aEnv );
static void ExecuteElementOperatorHelper( int nAccessModus, minInterpreterValue & aLeftVal, minInterpreterValue & aReturnValOut, minInterpreterNode * pRightNode, minInterpreterEnvironment & aEnv );

const double g_dMaxDouble			= 1e308;

#endif
