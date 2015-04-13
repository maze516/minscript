/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minparser.h,v $
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

#ifndef _MINPARSER_H
#define _MINPARSER_H

//*************************************************************************
#include "mstl_string.h"

//*************************************************************************

#include "dllexport.h"

//*************************************************************************
class minToken;
class minInterpreterNode;

// ParserItem-Typen:
//   * Expression (z.B. a = b*3+c/7;)
//   * Call (z.B. f(x))
//   * Jump

//   * Term
//   * Subterm
//   * Parameterlist
//   * Identifier
//   * Expression

// +++ einige Error-Codes die vom Parser gesetzt werden +++
const int PARSER_ERROR_IN_TYPEDEF					= 2000;
const int PARSER_ERROR_IN_WHILE						= 2001;
const int PARSER_ERROR_IN_DO						= 2002;
const int PARSER_ERROR_IN_FOR						= 2003;
const int PARSER_ERROR_IN_IF						= 2004;
const int PARSER_ERROR_IN_VAR_DECLARATION			= 2005;
const int PARSER_ERROR_IN_FCN_DECLARATION			= 2006;
const int PARSER_ERROR_IN_BLOCK						= 2007;
const int PARSER_ERROR_IDENTIFIER_EXPECTED			= 2008;
const int PARSER_ERROR_IN_CLASSBLOCK				= 2009;
const int PARSER_ERROR_IN_MEMBERDATA				= 2010;
const int PARSER_ERROR_IN_OBJ_DECLARATION			= 2011;
const int PARSER_ERROR_DESTRUCTOR_WITH_ARGUMENTS	= 2012;
const int PARSER_ERROR_DESTRUCTOR_ALLREADY_DEFINED	= 2013;
const int PARSER_ERROR_INTERNAL_ERROR				= 2014;
const int PARSER_ERROR_IN_TYPEOF					= 2015;
const int PARSER_ERROR_DOT_DOT_EXPECTED				= 2016;
const int PARSER_ERROR_TYPE_EXPECTED				= 2017;
const int PARSER_ERROR_IN_FCN_PTR_DECLARATION		= 2018;
const int PARSER_ERROR_IN_CONSTRUCTOR_INIT			= 2019;
const int PARSER_ERROR_IN_SWITCH					= 2020;
const int PARSER_ERROR_IN_CASE						= 2021;
const int PARSER_ERROR_CONSTANT_EXPECTED			= 2022;
const int PARSER_ERROR_TO_MANY_DEFAULT_LABELS		= 2023;
const int PARSER_ERROR_IN_SIZEOF					= 2024;
const int PARSER_ERROR_IN_TEMPLATE					= 2025;
const int PARSER_ERROR_IN_TEMPLATE_ARGS				= 2026;

//*************************************************************************

class minInterpreterNode;
class minVariableDeclarationNode;

#include "minparsernodes.h"

//*************************************************************************
class minVariableSymbolItem
{
public:
	// Konstruktor fuer eine Variable
	minVariableSymbolItem( const string & sSymbolName = "", minInterpreterType aType = Unknown )
		: m_sSymbolName( sSymbolName ), m_aType( aType )
	{}

	minInterpreterType		GetType() const			{ return m_aType; }

	const string &			GetSymbolName() const	{ return m_sSymbolName; }

#if defined( _MSC_VER ) 
	// Dummy-Implementationen
	bool operator<( const minVariableSymbolItem & aOther ) const	{ return m_sSymbolName < aOther.m_sSymbolName; }
	bool operator>( const minVariableSymbolItem & aOther ) const	{ return m_sSymbolName > aOther.m_sSymbolName; }
	bool operator==( const minVariableSymbolItem & aOther ) const	{ return m_sSymbolName == aOther.m_sSymbolName; }
	bool operator!=( const minVariableSymbolItem & aOther ) const	{ return !(*this==aOther); }
#endif

private:
	// Symbol Name
	string					m_sSymbolName;
	// Type Info
	minInterpreterType		m_aType;
};

//*************************************************************************
class MINDLLEXPORT minParser
{
	friend class minPrivateParser;

	typedef vector<const minClassDeclarationNode *>		MyUserTypeContainer;
	typedef list<minVariableSymbolItem>					MyVariableSymbolStack;
	typedef vector< pair<minInterpreterType,string> >	MyTypedefContainer;

public:
	minParser( minTokenizer * const pTokenizer );
	~minParser();

	// der Parseprozess wird ausgefuehrt:
	// der uebergebene Tokenizer wird zurueckgesetzt und
	// fuer die vom Tokenizer gelieferten Tokens wird
	// eine Liste von Parser-Items aufgebaut.
	// Diese Liste kann von einem minCodeProcessor abgearbeitet werden.
	bool Parse();
	// liefert den Parser-Knoten, der das Programm raepraesentiert
    minInterpreterNode * const GetProgramNode() const	{ return m_pProgramNode; }

	bool IsError() const						{ return m_bIsError; }
	int GetErrorCode() const					{ return m_nErrorCode; }

private:
	void Init();
	void SetError( int nCode )
	{
		m_bIsError = true;
		m_nErrorCode = nCode; 
		HandleError();
	}
	void HandleError();
	void SetProgramNode( minInterpreterNode * pNewProgNode );	

	// Methode zum Ignorieren von Whitespaces
	bool PeekRealToken( minToken & aTokenOut );
	minInterpreterType ReadAndSubstitueTypeToken( bool bIsConst, const string * psActClassName, StringListT & aTemplateTypeNameListOut );
	bool ReadToken()							{ return m_pTokenizer->ReadNextToken(); }

	void GetRefAndPointerLevel( int & nPointerLevel, bool & bIsReference );

	bool ParseFunction();
	bool ParseStatement( minInterpreterNode * & pNodeOut, const string * psClassName = 0 );
	bool ParseBlock( minInterpreterNode * & pNodeOut, bool bNoBlockIsError = true );
	bool PeekAndReadClassScope( InterpreterClassScope & aClassScopeInOut );
	bool ParseClass( bool bIsStruct, minInterpreterNode * & pNodeOut, string & sClassNameOut );
	bool ParseClassBlock( const string & sClassName, minClassBlockNode * & pNodeOut, bool bIsStruct );
	bool ParseForStringToken( const string & sStrg );
	bool ParseTypedef( minInterpreterNode * & pNodeOut );
	bool ParseCaseOrDefaultForSwitch( minInterpreterNode * & pNodeOut, bool & bIsDefaultOut );
	bool ParseSwitch( minInterpreterNode * & pNodeOut );
	bool ParseWhile( minInterpreterNode * & pNodeOut );
	bool ParseDo( minInterpreterNode * & pNodeOut );
	bool ParseFor( minInterpreterNode * & pNodeOut );
	bool ParseIf( minInterpreterNode * & pNodeOut );
	bool ParseArgumentDeclarationList( minVariableDeclarationList & aVarDeclList );
	bool ParseVarDeclarationOrFunction( minInterpreterNode * & pNodeOut, bool bIsConst, bool bIsVirtual, const string * psClassName = 0, bool bIsDestructor = false );
	bool ParseConstructorInitList( const string & sClassName, minParserItemList & aInitListOut );
	bool ParseKeyword( const minToken & aPeekedToken, minInterpreterNode * & pNodeOut, const string * psClassName );
	bool ParseArgumentList( minParserItemList & aArgumentList );
	bool ParseExpression( minInterpreterNode * & pExpression, bool bStopWithComma = false, bool bStopWithSemicolon = false, bool bStopWithArrayStop = false );
	bool ParseSizeof( minInterpreterNode * & pNodeOut );
	bool ParseTemplate( minInterpreterNode * & pNodeOut );
	bool ParseTemplateArgumentList( StringListT & aTypeNameListOut );

	void SkipWhitespaces();

	bool RegisterTypedefItem( const minInterpreterType & aOldType, const string & aNewTypeString );
	bool IsTypedefType( const string & aTypeString ) const;
	bool GetTypedefSubstitution( const string & aTypeStringIn, minInterpreterType & aOrgTypeOut ) const;

	// zur Verwaltung von class und struct Deklarationen (vom User angelegte Datentypen)
	bool RegisterUserType( const minClassDeclarationNode * pNewUserType );
	bool IsTemplateType( const string & sName ) const;
	bool IsUserType( const string & sName ) const;
	bool IsUserTypeDefined( const string & sName ) const;
	bool IsBuildInType( const minToken & aPeekedToken ) const;
	bool IsTokenValidType( const minToken & aPeekedToken ) const;
	bool IsVariableScopeModifier( const minToken & aToken );
	bool IsUnaryOperator( const minToken & aToken ) const;

	InterpreterValueType GetTypeFromString( const string & aTypeString, const string * psActClassName = 0 ) const;

	// +++ Daten +++
	minTokenizer * const	m_pTokenizer;			// KEIN Eigentuemer !
	minInterpreterNode *	m_pProgramNode;			// ist Eigentuemer !
	bool					m_bIsError;
	int						m_nErrorCode;
	MyUserTypeContainer		m_aUserTypeContainer;
	MyTypedefContainer		m_aTypedefContainer;	// neu seit 25.12.1999: Typedef Ersetzungen merken
	MyVariableSymbolStack	m_aVarSymbolStack;		// neu seit 24.12.1999: Symboltabelle, beachtet Sichtbarkeitsbereich: Name <--> Typ

	// temoraere Variable, wird benoetigt um Template-Klassen zu parsen
	StringListT				m_aTemplateTypeNameList;	
};

#endif
