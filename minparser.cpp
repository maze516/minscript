/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minparser.cpp,v $
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

#include "platform.h"

#include "mstl_iostream.h"		// fuer cout
#include "mstl_algorithm.h"		// fuer find()

#include <stdio.h>

#include "mintoken.h"
#include "mintokenizer.h"
#include "minipenv.h"			// fuer: minInterpreterEnvironment fuer Eval zur Compilezeit
#include "minparser.h"
#include "minparsernodes.h"

//*************************************************************************

inline bool IsInheritanceToken( const minToken & aTempToken )
{
	if( aTempToken.IsKeyword() && 
		(aTempToken.GetId()==PRIVATE_ID || aTempToken.GetId()==PROTECTED_ID || aTempToken.GetId()==PUBLIC_ID) )
		return true;
	return false;
}

//*************************************************************************
//*************************************************************************
//*************************************************************************

minParser::minParser( minTokenizer * const pTokenizer )
	: m_pTokenizer( pTokenizer ), 
      m_pProgramNode( 0 ), 
      m_bIsError( false ), 
      m_nErrorCode( 0 )
{
}

minParser::~minParser()
{
	delete m_pProgramNode;
}

void minParser::Init()
{
	m_bIsError = false;
	m_nErrorCode = ERROR_NO_ERROR;
}

bool minParser::Parse( int nLineCountOfAddedCode)
{
	m_pTokenizer->InitProcessing( nLineCountOfAddedCode );

	minInterpreterNode * pNode = 0;
	// WICHTIG: Script muss in einem Block stehen, z.B. "{ a = 1; }"
	if( ParseBlock( pNode ) )
	{
		SetProgramNode( pNode ); 
		return true;
	}
	return false;
}

bool minParser::ParseFunction()
{
	m_pTokenizer->InitProcessing( 0 );

	SkipWhitespaces();

	minInterpreterNode * pNode = 0;
	if( ParseVarDeclarationOrFunction( pNode, /*bIsConst*/false, /*bIsVirtual*/false ) )
	{
		SetProgramNode( pNode ); 
		return true;
	}
	return false;
}

// Hilfsmethode
bool minParser::ParseForStringToken( const string & sStrg, minToken * pToken )
{
	minToken aToken;

	SkipWhitespaces();

	if (PeekRealToken(aToken))
	{
		if( aToken.GetString() == sStrg )
		{
			ReadToken();
            if( pToken )
            {
                *pToken = aToken;
            }
			return true;
		}
	}
	return false;
}

bool minParser::ParseStatement( minInterpreterNode * & pNodeOut, const string * psClassName )
{
	minToken aToken;
	bool bOk = true;

	SkipWhitespaces();

	if (bOk && !m_pTokenizer->IsError() && PeekRealToken(aToken))
	{
		//cout << aToken.GetString() << endl;

		if( aToken.IsKeyword() && !IsVariableScopeModifier( aToken ) )
		{
			bOk = ParseKeyword( aToken, pNodeOut, psClassName );
		} 
		else if( IsUserType( aToken.GetString() ) || 
			     IsTemplateType( aToken.GetString() ) ||	// neu seit 14.2.2003
			     IsTypedefType( aToken.GetString() ) ||		// neu seit 14.11.1999, 25.12.1999
				 IsVariableScopeModifier( aToken ) ||										// neu seit 4.1.2000
				 (psClassName && (*psClassName==aToken.GetString())) ||						// neu seit 19.12.1999
				 (psClassName && aToken.IsOperator() && (aToken.GetId()==INVERT_ID)) )		// Destruktor Deklaration, neu seit 23.12.1999
		{
			// es ist eine Variablen Deklaration oder eine Destruktor-Deklaration !

			// static oder extern lesen, und auf extern "C" pruefen
			if( IsVariableScopeModifier( aToken ) )
			{
				ReadToken();

				// extern "C" behandeln
				if( aToken.GetId()==EXTERN_ID && PeekRealToken( aToken ) && aToken.IsString() && aToken.GetString()=="C" )
				{
					ReadToken();
				}

				// TODO: ggf. Zustand merken, damit er spaeter beim C++-Code-Generieren ausgegeben werden kann
			}

			// ist es eine Destruktor-Deklaration ?
			bool bIsDestructor = false;
			if( (psClassName && aToken.IsOperator() && (aToken.GetId()==INVERT_ID)) )
			{
				// ja --> 
				// ~ Token lesen und Destruktor-Flag merken
				m_pTokenizer->GetNextToken( aToken );
				bIsDestructor = true;
			}
			bOk = ParseVarDeclarationOrFunction( pNodeOut, /*bIsConst*/false, /*bIsVirtual*/false, psClassName, bIsDestructor );
		}
		else if( aToken.IsIdentifier() || aToken.IsConstant() || aToken.IsOperator() )
		{
			bOk = ParseExpression( pNodeOut );
		}
		else if( aToken.IsSpecialChar() )
		{
			if( aToken.IsBlockOpen() )
			{
				return ParseBlock( pNodeOut );
			}
			else if( aToken.IsStatementTerminator() )
			{
				ReadToken();
				pNodeOut = new minEmptyNode();
				return true;
			}
			else if( aToken.IsParenthisOpen() )		// Bugfix 4.1.2000: Expression kann auch in Klammern (...) stehen !
			{
				return ParseExpression( pNodeOut );
			}
			return false;
		}
		else if( aToken.IsComment() || aToken.IsWhitespace() )
		{
			ReadToken();
			pNodeOut = new minCommentNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo( aToken.GetLineNo() ) );
			return true;
		}
	}

	return bOk && !m_pTokenizer->IsError();
}

void minParser::SkipWhitespaces()
{
	// skip whitespace tokens
	bool bContinue = true;
	while (bContinue)
	{
		minToken aToken;
		if (PeekRealToken(aToken))
		{
			if (aToken.IsWhitespace())
			{
				ReadToken();
			}
			else
			{
				bContinue = false;
			}
		}

	}
}

bool minParser::ParseBlock( minInterpreterNode * & pNodeOut, bool bNoBlockIsError )
{
	minInterpreterNode * pNode = 0;

	SkipWhitespaces();

    minToken aToken;
    if( ParseForStringToken( "{", &aToken ) )
	{
		minParserItemList aNodeList;

		while( ParseStatement( pNode ) )
		{
			aNodeList.push_back( minHandle<minInterpreterNode>( pNode ) );
		}
		// ueberpruefe warum das Parsen der Liste abgebrochen wurde.
		// wurde das Statement-List-Terminator Token, z.B. } gefunden ?
		if( ParseForStringToken( "}" ) )
		{
            pNodeOut = new minBlockNode( aNodeList, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
			return true;
		}
	}
	else if( !bNoBlockIsError && ParseForStringToken( ";" ) )	// Statement end, wird fuer Funktions-Prototypen verwendet
	{
		pNodeOut = 0;
		return true;
	}
	pNodeOut = 0;
	// nur unbekannten Fehler setzen, wenn noch kein Fehler gesetzt ist (neu seit 14.11.1999) !
	if( !IsError() )
	{
		SetError( PARSER_ERROR_IN_BLOCK, aToken.GetLineNo() );
	}
	return false;
}

bool minParser::PeekAndReadClassScope( InterpreterClassScope & aClassScopeInOut )
{
	minToken aToken;

	SkipWhitespaces();

	// Bugfix 9.2.2003: aendere uebergebenen Class-Scope nicht !
	//aClassScopeInOut = None;

	// Bugfix 5.2.2003: lese so lange public:, private: etc. bis
	// es keine mehr gibt, liefere den letzten Zustand zurueck.
	while( PeekRealToken( aToken ) && IsInheritanceToken( aToken ) )
	{
		ReadToken();

		// Bugfix 5.2.2003
		switch( aToken.GetId() )
		{
			case PRIVATE_ID:
				aClassScopeInOut = Private;
				break;
			case PROTECTED_ID:
				aClassScopeInOut = Protected;
				break;
			case PUBLIC_ID:
				aClassScopeInOut = Public;
				break;

			default:
				assert( false );
		}

		// jetzt muss noch ein : kommen
		if( PeekRealToken( aToken ) && aToken.GetId()==DOT_DOT_ID )
		{
			ReadToken();
		}
		else
		{
			// ERROR: : erwartet !
			SetError( PARSER_ERROR_DOT_DOT_EXPECTED, aToken.GetLineNo() );
			return false;
		}
	}

	return true;
}

// Parsen einer Klassendeklaration (vorlaeufig nur Java-aehnlich)
bool minParser::ParseClassBlock( const string & sClassName, minClassBlockNode * & pNodeOut, bool bIsStruct )
{
	minInterpreterNode * pNode = 0;

	SkipWhitespaces();

	minToken aToken;
	if( PeekRealToken( aToken ) && !(aToken.IsBlockOpen() || aToken.IsStatementTerminator()) )
	{
		SetError( PARSER_ERROR_IN_CLASSBLOCK, aToken.GetLineNo() );
		pNodeOut = 0;
		return false;
	}

	// Vorausdeklaration behandeln, z.B. class MyClass; (neu seit 4.1.2000)
	if( aToken.IsStatementTerminator() )
	{
		ReadToken();

		// Vorausdeklarationen haben keine Implementation, dies wird im minClassDeclarationNode beachtet !
		pNodeOut = 0;
		return true;
	}
	// richtige Klassendeklaration behandeln
    else if( ParseForStringToken( "{", &aToken ) )
	{
		minParserItemList aConstructorList;
		minParserItemList aMethodList;
		minParserItemList aMemberVariableList;
		minHandle<minInterpreterNode> hDestructorNode;

		// hier duerfen nur Konstruktoren, ein Destruktor, Methoden und Daten (ohne Initialisierung) kommen
		InterpreterClassScope aClassScope = bIsStruct ? Public : Private;	// Structs sind immer public, Classes immer private
		while( PeekAndReadClassScope( aClassScope ) && ParseStatement( pNode, &sClassName ) )
		{
			const string & sNodeClassName = pNode->GetClassName();

			// eigene RTTI durchfuehren und Klasse des pNode bestimmen
			if( sNodeClassName==_VARIABLEDECLARATIONNODE )			// Member-Daten behandeln
			{
				// bei Member-Daten ist keine Initialisierung erlaubt...
				minVariableDeclarationNode * pVarNode = (minVariableDeclarationNode *)pNode;
				pVarNode->SetClassScope( aClassScope );
				if( !pVarNode->IsValidMemberDeclaration() )
				{
					SetError( PARSER_ERROR_IN_MEMBERDATA, aToken.GetLineNo() );
					return false;
				}

				aMemberVariableList.push_back( minHandle<minInterpreterNode>( pNode ) );
			}
			else if( sNodeClassName==_FUNCTIONDECLARATIONNODE )		// Methode behandeln
			{
				minFunctionDeclarationNode * pFcnNode = (minFunctionDeclarationNode *)pNode;
				// ist es ggf. ein Konstruktor ?
				if( pFcnNode->GetName()==sClassName )
				{
					aConstructorList.push_back( minHandle<minInterpreterNode>( pNode ) );
				}
				// ist es ggf. ein Destruktor ?
				else if( pFcnNode->GetName()==string("~")+sClassName )
				{
					if( hDestructorNode.IsOk() )
					{
						// Fehler: es wurde ein zweiter Destruktor angegeben !
						SetError( PARSER_ERROR_DESTRUCTOR_ALLREADY_DEFINED, aToken.GetLineNo() );
						return false;
					}
					else
					{
						hDestructorNode = minHandle<minInterpreterNode>( pNode );
					}
				}
				// ansonsten muss es eine Methode sein !
				else
				{
					aMethodList.push_back( minHandle<minInterpreterNode>( pNode ) );
				}
				// WICHTIG: Methoden werden IMMER mit dem Klassennamen 'verziert':
				//			d.h. MyClass::f( int ) --> MyClass_f( int )
				pFcnNode->SetNewMethodName( MakeMethodName( sClassName, pFcnNode->GetName() ), aClassScope );
			}
			else if( sNodeClassName==_EMPTYNODE )		// Bugfix: 19.1.2000
			{
				// Statement-Terminatoren ignorierern
			}
			else
			{
				// sollte NIE auftreten !
				SetError( PARSER_ERROR_INTERNAL_ERROR, aToken.GetLineNo() );
				return false;
			}
		}
		// ueberpruefe warum das Parsen der Liste abgebrochen wurde.
		// wurde das Statement-List-Terminator Token, z.B. } gefunden ?
		if( ParseForStringToken( "}" ) )
		{
			// jetzt noch das Semikolon lesen
			//ParseForStringToken( ";" )

			// jetzt noch ggf. den Default-Konstruktor bzw. Default-Destruktor anlegen (mit leerem Block)
			if( aConstructorList.size()==0 )
			{
                aConstructorList.push_back( minHandle<minInterpreterNode>( new minInterpreterFunctionDeclarationNode( sClassName+g_sClassMethodSeparator+sClassName, /*bIsConstant*/false, /*bIsVirtual*/false, Void, minVariableDeclarationList(), /*pCode*/new minBlockNode( minParserItemList(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) ), minParserItemList(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) ) ) );
			}
			if( !hDestructorNode.IsOk() )
			{
                hDestructorNode = minHandle<minInterpreterNode>( new minInterpreterFunctionDeclarationNode( sClassName+g_sClassMethodSeparator+"~"+sClassName, /*bIsConstant*/false, /*bIsVirtual*/false, Void, minVariableDeclarationList(), /*pCode*/new minBlockNode( minParserItemList(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) ), minParserItemList(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) ) );
			}

            pNodeOut = new minClassBlockNode( aMethodList, aMemberVariableList, aConstructorList, hDestructorNode, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );

			return true;
		}
	}
	else 
	{
		SetError( PARSER_ERROR_IN_CLASSBLOCK, aToken.GetLineNo() );
		pNodeOut = 0;
		return false;
	}
	pNodeOut = 0;
	// nur unbekannten Fehler setzen, wenn noch kein Fehler gesetzt ist (neu seit 14.11.1999) !
	if( !IsError() )
	{
		SetError( PARSER_ERROR_IN_CLASSBLOCK, aToken.GetLineNo() );
	}
	return false;
}

bool minParser::ParseTypedef( minInterpreterNode * & pNodeOut )
{
	// gepeektes (typedef) Token lesen 
	m_pTokenizer->ReadNextToken();

	// nach typedef duerfen genau zwei 'Identifier'-Token folgen und ein Statement-End-Token (;)
	//   typedef [const] AlterTypName [*] [*] ... [&] NeuerTypeName;
	// oder struct bzw. class Deklarationen
	//   typedef struct MyClass { ... } MyClass;
	// oder Funktionszeiger Definition
	//   typedef int (*pFcnT)( ... );
	//   typedef int (pFcnT)( ... );
	// oder Funktionszeiger Definition
	//   typedef int pFcnT( ... );			// wird zur Zeit noch NICHT unterstuezt !
	minToken aOldTypeToken;
	bool bIsConst = false;
	// verarbeite ein const
	if( PeekRealToken( aOldTypeToken ) && aOldTypeToken.IsKeyword() && aOldTypeToken.GetId()==CONST_ID )
	{
		ReadToken();
		bIsConst = true;
	}
	// Behandlung von einfachen Typdef's und Funktionszeiger-Typedefs
	if( PeekRealToken( aOldTypeToken ) && IsTokenValidType( aOldTypeToken ) )
	{
		ReadToken();

		// jetzt koennen mehrere Pointer-Operatoren (*) und ein Referenz-Operator (&) folgen
		int nPointerLevel = 0;
		bool bIsReference = false;
		GetRefAndPointerLevel( nPointerLevel, bIsReference );

		minToken aNewTypeToken;
		if( PeekRealToken( aNewTypeToken ) && aNewTypeToken.IsParenthisOpen() )
		{
			// dann ist es eine Funktionszeiger Definition der Art: typedef int(*pFcnT)();
			ReadToken();

			// jetzt kann ein * kommen
			if( PeekRealToken( aNewTypeToken ) && aNewTypeToken.IsOperator() && aNewTypeToken.GetId()==MULT_ID )
			{
				ReadToken();
			}

			// Typ-Bezeichner lesen
			if( PeekRealToken( aNewTypeToken ) )
			{
				ReadToken();

				if( !IsBuildInType( aNewTypeToken ) )
				{
					minInterpreterType aOldType( Function, /*nPointerLevel*/0, /*bIsReference*/0, "", /*bIsConst*/false );
					string sNewTypeName = aNewTypeToken.GetString();

					if( PeekRealToken( aNewTypeToken ) && aNewTypeToken.IsParenthisClose() )
					{
						ReadToken();

						// dann muss eine Parameter-Deklaration folgen
						minVariableDeclarationList aVarDeclList;
						if( ParseArgumentDeclarationList( aVarDeclList ) )
						{
							if( ParseForStringToken( ";" ) && RegisterTypedefItem( aOldType, sNewTypeName ) )
							{
                                pNodeOut = new minTypedefNode( aOldType, sNewTypeName, 0, minDebuggerInfo::CreateDebuggerInfo( aNewTypeToken.GetLineNo() ) );
								return true;
							}
						}
					}
				}
			}
		}
		else
		{
			// dann ist es eine normale Typen Definition
			ReadToken();

			if( !IsBuildInType( aNewTypeToken ) )
			{
				// TODO: ggf. hier noch templates behandeln ! Bisher: verwende #define's
				// Beispiel: typedef list<int> IntList;

				// Bem.: im typedef-Kontainer werden Namen und PointerLevel, Reference getrennt gespeichert !
				minInterpreterType aOldType( GetTypeFromString( aOldTypeToken.GetString() ), nPointerLevel, bIsReference, aOldTypeToken.GetString(), bIsConst );
				if( ParseForStringToken( ";" ) && RegisterTypedefItem( aOldType, aNewTypeToken.GetString() ) )
				{
                    pNodeOut = new minTypedefNode( aOldType, aNewTypeToken.GetString(), 0, minDebuggerInfo::CreateDebuggerInfo( aNewTypeToken.GetLineNo() ) );
					return true;
				}
			}
		}
	}
	// Behandlung der Klassendeklaration
	if( aOldTypeToken.IsKeyword() && (aOldTypeToken.GetId()==CLASS_ID || aOldTypeToken.GetId()==STRUCT_ID) )
	{
		ReadToken();

		minInterpreterNode * pClassDeclarationCode = 0;
		string sClassName;

		// TODO: hier behandeln:
		//	typedef struct MyStruct * MyTypeP;
		//	typedef struct MyStruct MyStruct;

		if( ParseClass( /*bIsStruct*/aOldTypeToken.GetId()==STRUCT_ID, pClassDeclarationCode, sClassName ) )
		{
			minToken aNewTypeToken;
			if( PeekRealToken( aNewTypeToken ) )
			{
				ReadToken();

				if( !IsBuildInType( aNewTypeToken ) )
				{
					// Bem.: im typedef-Kontainer werden Namen und PointerLevel, Reference getrennt gespeichert !
					minInterpreterType aOldType( Object, 0, false, sClassName );
					if( ParseForStringToken( ";" ) && RegisterTypedefItem( aOldType, aNewTypeToken.GetString() ) )
					{
                        pNodeOut = new minTypedefNode( aOldType, aNewTypeToken.GetString(), pClassDeclarationCode, minDebuggerInfo::CreateDebuggerInfo( aNewTypeToken.GetLineNo() )  );
						return true;
					}
				}
			}
		}
		else
		{
			// Fehler wird schon in ParseClass behandelt ...
			return false;
		}
		return true;
	}

	pNodeOut = 0;
	SetError( PARSER_ERROR_IN_TYPEDEF, -1 );
	return false;
}

bool minParser::ParseCaseOrDefaultForSwitch( minInterpreterNode * & pNodeOut, bool & bIsDefaultOut )
{
	SkipWhitespaces();

	// case constant :
	//    statement-list
	//    break;

	minToken aToken;
	if( PeekRealToken( aToken ) && (aToken.GetId()==CASE_ID || aToken.GetId()==DEFAULT_ID) )
	{
		bIsDefaultOut = (aToken.GetId()==DEFAULT_ID);

		ReadToken();

		minInterpreterNode * pConstExprNode = 0;
		if( !bIsDefaultOut )
		{
			// jetzt muss eine Konstante kommen, falls es kein default-Label ist !
			/*
			PeekRealToken( aToken );
			if( !aToken.IsConstant() || aToken.IsIdentifier() )
			{
				pNodeOut = 0;
				SetError( PARSER_ERROR_CONSTANT_EXPECTED );
				return false;
			}
			*/
			if( !ParseExpression( pConstExprNode, /*bStopWithComma*/false, /*bStopWithSemicolon*/false, /*bStopWithArrayStop*/false ) )
			{
				pNodeOut = 0;
				SetError( PARSER_ERROR_CONSTANT_EXPECTED, aToken.GetLineNo() );
				return false;
			}
		}

		// jetzt muss ein : kommen
		PeekRealToken( aToken );
		if( aToken.GetId()!=DOT_DOT_ID )
		{
			pNodeOut = 0;
			SetError( PARSER_ERROR_DOT_DOT_EXPECTED, aToken.GetLineNo() );
			return false;
		}
		ReadToken();

		// jetzt kommt eine Liste von Statements
		minParserItemList		aNodeList;
		minInterpreterNode *	pNode = 0;

		while( ParseStatement( pNode ) )
		{
			aNodeList.push_back( minHandle<minInterpreterNode>( pNode ) );
		}

        pNodeOut = new minCaseLabelNode( pConstExprNode, aNodeList, minDebuggerInfo::CreateDebuggerInfo( aToken.GetLineNo() ) );
		return true;
	}

	pNodeOut = 0;
	// keinen Fehler ausgeben, da irgendwann ein switch-Statement
	// mit einer } beendet wird, d.h. kein weiterer case-Label
	// folgen kann.
	//SetError( PARSER_ERROR_IN_CASE );
	return false;
}

bool minParser::ParseSwitch( minInterpreterNode * & pNodeOut )
{
	SkipWhitespaces();

	// gepeektes (switch) Token lesen 
    minToken aSwitchToken;
    m_pTokenizer->GetNextToken(aSwitchToken);

	// Bedingung in Klammern parsen
	minInterpreterNode * pExpression = 0;
	if( ParseForStringToken( "(" ) && 
		ParseExpression( pExpression, /*bStopWithComma*/false, /*bStopWithSemicolon*/true ) &&
		ParseForStringToken( ")" ) )
	{
		if( ParseForStringToken( "{" ) )
		{
			minParserItemList		aNodeList;
			minInterpreterNode *	pCaseNode = 0;
			bool					bIsDefaultFound = false;
			bool					bIsDefault = false;

			while( ParseCaseOrDefaultForSwitch( pCaseNode, bIsDefault ) )
			{
				// es darf nur ein default Label geben !
				if( bIsDefault && bIsDefaultFound )
				{
					pNodeOut = 0;
					SetError( PARSER_ERROR_TO_MANY_DEFAULT_LABELS, aSwitchToken.GetLineNo() );
					return false;
				}
				if( bIsDefault )
				{
					bIsDefaultFound = true;
				}
				aNodeList.push_back( minHandle<minInterpreterNode>( pCaseNode ) );
			}
			// ueberpruefe warum das Parsen der Liste abgebrochen wurde.
			// wurde das Statement-List-Terminator Token, z.B. } gefunden ?
			if( ParseForStringToken( "}" ) )
			{
                pNodeOut = new minSwitchNode( pExpression, aNodeList, minDebuggerInfo::CreateDebuggerInfo(aSwitchToken.GetLineNo()) );
				return true;
			}
		}
	}
	pNodeOut = 0;
	SetError( PARSER_ERROR_IN_SWITCH, aSwitchToken.GetLineNo() );
	return false;
}

bool minParser::ParseWhile( minInterpreterNode * & pNodeOut )
{
	SkipWhitespaces();

	// gepeektes (while) Token lesen 
    minToken aWhileToken;
    m_pTokenizer->GetNextToken( aWhileToken );

	// Bedingung in Klammern parsen
	minInterpreterNode * pExpression = 0;
	if( ParseForStringToken( "(" ) && 
		ParseExpression( pExpression, /*bStopWithComma*/false, /*bStopWithSemicolon*/true ) &&
		ParseForStringToken( ")" ) )
	{
		// jetzt das Statement fuer das while parsen, kann auch ein Block sein
		minInterpreterNode * pStatement = 0;
		if( ParseStatement( pStatement ) )
		{
            pNodeOut = new minWhileNode( pExpression, pStatement, minDebuggerInfo::CreateDebuggerInfo(aWhileToken.GetLineNo()) );
			return true;
		}
	}
	pNodeOut = 0;
	SetError( PARSER_ERROR_IN_WHILE, aWhileToken.GetLineNo() );
	return false;
}

bool minParser::ParseDo( minInterpreterNode * & pNodeOut )
{
	SkipWhitespaces();

	// gepeektes (do) Token lesen 
    minToken aDoToken;
    m_pTokenizer->GetNextToken(aDoToken);

	// jetzt das Statement nach dem do parsen, MUSS ein Block sein
	minInterpreterNode * pStatement = 0;
	if( ParseBlock( pStatement ) )
	{
		// jetzt muss ein while folgen
		minToken aToken;

		// jetzt koennte ein else kommen
		if( PeekRealToken( aToken ) && aToken.IsKeyword() && aToken.GetId()==WHILE_ID )
		{
			// gepeektes (while) Token lesen
			m_pTokenizer->ReadNextToken();

			// Bedingung in Klammern parsen
			minInterpreterNode * pExpression = 0;
			if( ParseForStringToken( "(" ) && 
				ParseExpression( pExpression, /*bStopWithComma*/false, /*bStopWithSemicolon*/true ) && 
				ParseForStringToken( ")" ) && 
				ParseForStringToken( ";" ) )
			{
                pNodeOut = new minDoNode( pExpression, pStatement, minDebuggerInfo::CreateDebuggerInfo(aDoToken.GetLineNo()) );
				return true;
			}
		}
	}
	pNodeOut = 0;
	SetError( PARSER_ERROR_IN_DO, aDoToken.GetLineNo() );
	return false;
}

bool minParser::ParseFor( minInterpreterNode * & pNodeOut )
{
	SkipWhitespaces();

	// gepeektes (for) Token lesen 
    minToken aForToken;
    m_pTokenizer->GetNextToken(aForToken);

	// Init, Bedingung und Loop Anweisung in Klammern parsen
	minInterpreterNode * pInitExpression = 0;
	minInterpreterNode * pCheckExpression = 0;
	minInterpreterNode * pLoopExpression = 0;
	if( ParseForStringToken( "(" ) && 
		ParseExpression( pInitExpression ) &&		
		ParseExpression( pCheckExpression ) &&
		ParseExpression( pLoopExpression, /*bStopWithComma*/false, /*bStopWithSemicolon*/true ) && 
		ParseForStringToken( ")" ) )
	{
		// jetzt das Statement fuer das while parsen, kann auch ein Block sein
		minInterpreterNode * pStatement = 0;
		if( ParseStatement( pStatement ) )
		{
            pNodeOut = new minForNode( pInitExpression, pCheckExpression, pLoopExpression, pStatement, minDebuggerInfo::CreateDebuggerInfo(aForToken.GetLineNo()) );
			return true;
		}
	}
	pNodeOut = 0;
	SetError( PARSER_ERROR_IN_FOR, aForToken.GetLineNo() );
	return false;
}

bool minParser::ParseIf( minInterpreterNode * & pNodeOut )
{
	// auf jeden Fall den Output-Parameter initialisieren
	pNodeOut = 0;

	SkipWhitespaces();

	// gepeektes (if) Token lesen
    minToken aIfToken;
    m_pTokenizer->GetNextToken(aIfToken);

	// Bedingung in Klammern parsen
	minInterpreterNode * pExpression = 0;
	if( ParseForStringToken( "(" ) && ParseExpression( pExpression ) && ParseForStringToken( ")" ) )
	{
		// jetzt den then-Zweig parsen
		minInterpreterNode * pThenStatement = 0;
		minInterpreterNode * pElseStatement = 0;
		if( ParseStatement( pThenStatement ) )
		{
			minToken aToken;

			SkipWhitespaces();

			// jetzt koennte ein else kommen
			if( PeekRealToken( aToken ) && aToken.IsKeyword() && aToken.GetId()==ELSE_ID )
			{
				// gepeektes (else) Token lesen
				m_pTokenizer->ReadNextToken();

				// und den else-Zweig parsen
				if( !ParseStatement( pElseStatement ) )
				{
					SetError( PARSER_ERROR_IN_IF, aToken.GetLineNo() );
					return false;
				}
			}

            pNodeOut = new minIfNode( pExpression, pThenStatement, pElseStatement, minDebuggerInfo::CreateDebuggerInfo(aIfToken.GetLineNo()) );
			return true;
		}
	}
	SetError( PARSER_ERROR_IN_IF, aIfToken.GetLineNo() );
	return false;
}

// Hilfsfunktion um die Argument-Liste einer Funktionsdeklaration zu parsen, z.B. int f( int i, double d ) ...
// Vorbedingung: das erste Token ( darf noch nicht aus dem Tokenizer gelesen sein !
// Nachbedingung: die gesamte Klammer, inklusive des schliessenden ) wird gelesen,
//				  es wird true geliefert wenn alles ok, ansonsten false !
bool minParser::ParseArgumentDeclarationList( minVariableDeclarationList & aVarDeclList )
									   /*ACHTUNG: hier werden ggf. NEUE Objekte hinzugefuegt ! */
{
	minToken aToken;
	bool bContinue = true;
	int nNoOfArguments = 0;

	// es muss noch das ( gelesen werden
	if( PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
	{
		ReadToken();

		bool bIsConst = false;

		// nur wenn ( gefunden wurde weiter in der Behandlung...
		while( bContinue && PeekRealToken( aToken ) )
		{
			// eine Argument-Declaration muss immer mit einem Typ beginnen:
			// z.B. int f( int x, double y )
			// oder schliesst mit einem ) Token
			if( aToken.IsParenthisClose() )
			{
				bContinue = false;
				ReadToken();		// lese auch abschliessendes )
			}
			else if( aToken.IsKeyword() && aToken.GetId()==CONST_ID )
			{
				ReadToken();
				bIsConst = true;
			}
			else if( aToken.IsKeyword() && (aToken.GetId()==STRUCT_ID || aToken.GetId()==STRUCT_ID) )	// Bugfix: 19.1.2000
			{
				// es kann z.B. stehen: int f( struct MyStruct * p );
				ReadToken();
			}
			else if( IsTokenValidType( aToken ) )
			{
				nNoOfArguments++;

				StringListT aTemplateTypeNameList;

				// Typ Token inkl. * und & Tokens komplett lesen
				minInterpreterType aIpType = ReadAndSubstitueTypeToken( bIsConst, 0, aTemplateTypeNameList );

				// jetzt MUSS ein einfacher Identifier kommen
				if( PeekRealToken( aToken ) && (aToken.IsIdentifier() || (aToken.GetId()==COMMA_ID) || aToken.IsParenthisClose()) )
				{
					string sVariableName = aToken.GetString();
					if( aToken.IsIdentifier() )
					{
						// gepeektes (Identifier) Token auch lesen
						ReadToken();
					}
					else
					{
						// falls kein Identifier angegeben ist (z.B. int f( int );), einen Default-Namen erzeugen
						char sBuffer[c_iMaxBuffer];
						sprintf( sBuffer, "___par_%d", nNoOfArguments );
						sVariableName = sBuffer;
					}

					// neue Variablen-Deklaration in die Variablen-Deklarationsliste eintragen
					minVariableDeclarationNode * pVarNode 
						= new minVariableDeclarationNode(sVariableName, aIpType, /*nArraySize*/-1, 0, 0, 0, StringListT(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()));
					// und neue Variablendeklaration in die Liste eintragen
					aVarDeclList.push_back( minHandle<minVariableDeclarationNode>( pVarNode ) );

					// jetzt muss das const Flag wieder zurueckgesetzt werden !
					bIsConst = false;
				}
				else
				{
					SetError( PARSER_ERROR_IDENTIFIER_EXPECTED, aToken.GetLineNo() );	// wird z.Z. zwar ueberschriebenm ... !
					return false;
				}
				
				// jetzt koennte ein , oder ein ) folgen
				if( PeekRealToken( aToken ) && (aToken.GetId()==COMMA_ID) )
				{
					// Komma gefunden es geht einfach weiter
					// gepeektes (Identifier) Token auch lesen
					ReadToken();
				}
				// das ) wird im naechsten Schleifendurchlauf behandelt !
			}
			else
			{
				// unbekannter Datentyp gefunden (Bugfix: 4.1.2000)
				SetError( PARSER_ERROR_TYPE_EXPECTED, aToken.GetLineNo() );
				return false;
			}
		}	// end of while()

		// trat ein Fehler auf oder ist die Variablen-Deklaration einfach (korrekt) beendet ?
		return aToken.IsParenthisClose();	
	}

	return false;
}

bool minParser::ParseConstructorInitList( const string & sClassName, minParserItemList & aInitListOut )
{
	// vorlaeufig nur Konstruktor-Calls zulassen
	// Beispiel: ClassA() : Class1(3,4), Class2("abc") {}
	// d.h. doppelte Loop (Klassen und Argumente) notwendig

	minToken aToken;
	bool bContinue = true;
	while( bContinue )
	{
		// parse einen Ausdruck von der Art: Class1(3,4)
		// d.h. erst Identifier, gefolgt von ( dann eine Liste von Ausdruecken
		// jeweils durch , getrennt und ein abschliessendes )
		if( PeekRealToken( aToken ) && aToken.IsIdentifier() )
		{
			ReadToken();

			string sBaseClass = aToken.GetString();

			//if( false /*!IsBaseClass( sClassName, aToken.GetString() )*/ )
			//{
			//	// ERROR
			//	SetError( ERROR_IN_BASECLASS_INIT );
			//	return false;
			//}

			// jetzt muss eine Klammer mit einer Liste von Ausdruecken folgen:
			if( PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
			{
				ReadToken();

				minParserItemList aArgumentList;

				// die Liste der Ausdrucke abarbeiten, es kann auch kein Ausdruck stehen,
				// d.h. Beispiel: Class1()
				while( PeekRealToken( aToken ) && !aToken.IsParenthisClose() )
				{
					minInterpreterNode * pExpression = 0;
					if( ParseExpression( pExpression, /*bStopWithComma*/true, /*bStopWithSemicolon*/true, /*bStopWithArrayStop*/false ) )
					{
						aArgumentList.push_back( minHandle<minInterpreterNode>( pExpression ) );

						bool bPeekOk = PeekRealToken( aToken );

						if( bPeekOk && !aToken.IsParenthisClose() )
						{
							if( bPeekOk && (aToken.GetId()==COMMA_ID) )
							{
								ReadToken();
							}
							else
							{
								SetError( ERROR_IN_BASECLASS_INIT, aToken.GetLineNo() );
								return false;
							}
						}
					}
					else
					{
						SetError( ERROR_IN_BASECLASS_INIT, aToken.GetLineNo() );
						return false;
					}
				}
				if( aToken.IsParenthisClose() )
				{
					ReadToken();
				}

                aInitListOut.push_back( minHandle<minInterpreterNode>( new minFunctionCallNode( sBaseClass, aArgumentList, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) ) ) );
			}
			else
			{
				SetError( ERROR_IN_BASECLASS_INIT, aToken.GetLineNo() );
				return false;
			}
		}

		// hier koennte es mit weiteren Basisklassen-Konstruktoren weiter gehen,
		// jeweils durch ein Komma getrennt !
		// Beispiel: ClassA() : Class1(3,4), Class2("abc") {}

		// geht es weiter, dann kommt ein Komma
		if( PeekRealToken( aToken ) && (aToken.GetId()==COMMA_ID) )
		{
			ReadToken();
		}
		else
		{
			bContinue = false;
		}

	}
	return true;
}

// das class bzw. struct Token wurde schon aus dem Tokenizer gelesen
// es duerfen folgen: Variablen-Deklarationen, allerdings OHNE Initialisierung (verboten: int i = 0;) !!!
// es duerfen folgen: Funktionen/Methoden
// Konstruktor und Destruktor (NEU)
bool minParser::ParseClass( bool bIsStruct, minInterpreterNode * & pNodeOut, string & sClassNameOut )
{
	// jetzt muesste ein Identifier folgen, dann ein Class-Block und zum Schluss ein Semikolon
	minToken aIdentiferToken;
	if( !PeekRealToken( aIdentiferToken ) || !aIdentiferToken.IsIdentifier() )
	{
		pNodeOut = 0;
		SetError(EXPECTED_CLASS_NAME, aIdentiferToken.GetLineNo());
		return false;
	}

	// gepeektes Identifier Token auch lesen
	m_pTokenizer->ReadNextToken();

	sClassNameOut = aIdentiferToken.GetString();

	// ist schon einen User-Typ mit dem angegebenen Namen definiert worden, d.h. eine Implementation angegeben?
	if( IsUserTypeDefined( sClassNameOut ) )
	{
		pNodeOut = 0;
		SetError(CLASS_NAME_ALREADY_DEFINED, aIdentiferToken.GetLineNo());
		return false;
	}

	// neu seit 22.1.2000
	// Behandlung von Vererbungsbeziehungen fuer Klassen
	// jetzt kann eine Liste von Basisklassen folgen,
	// Beispiel class MyClass : public MyBaseClass {}
	minBaseClassList aBaseClassList;
	minToken aTempToken;
	if( PeekRealToken( aTempToken ) && aTempToken.GetId()==DOT_DOT_ID )
	{
		// gepeektes : Token auch lesen
		ReadToken();

		bool bContinue = true;
		while( bContinue )
		{
			// Gueltigkeitsbezeichner (public, protected, private) lesen
			if( PeekRealToken( aTempToken ) && IsInheritanceToken( aTempToken ) )
			{
				// gepeektes Token auch lesen
				ReadToken();

				// Bemerkung: z.Z. wird der Typ der Vererbung ignoriert !
			}
			else
			{
				pNodeOut = 0;
				SetError( EXPECTED_INHERITANCE_TOKEN, aTempToken.GetLineNo() );
				return false;
			}

			// Basis-Klasse lesen
			if( PeekRealToken( aTempToken ) && aTempToken.IsIdentifier() )
			{
				// gepeektes Token auch lesen
				ReadToken();

				// ist die Basisklasse ueberhaupt bekannt ?
				if( !IsUserType( aTempToken.GetString() ) )
				{
					pNodeOut = 0;
					SetError( UNKNOWN_BASECLASS, aTempToken.GetLineNo() );
					return false;
				}

				// und Basisklasse merken
				aBaseClassList.push_back( aTempToken.GetString() );
			}
			else
			{
				pNodeOut = 0;
				SetError( EXPECTED_BASECLASS, aTempToken.GetLineNo() );
				return false;
			}

			// kommt nun ein , um weitere Basisklassen anzugeben ?
			if( PeekRealToken( aTempToken ) && (aTempToken.GetId()==COMMA_ID) )
			{
				// gepeektes , Token auch lesen
				ReadToken();
			}
			else
			{
				bContinue = false;
			}
		}
	}

	// Bugfix: 19.1.2000
	// TODO: hier behandeln:
	//	typedef struct MyStruct * MyTypeP;
	//	typedef struct MyStruct MyStruct;
	/*
	minToken aTempToken;
	if( PeekRealToken( aTempToken ) && (aTempToken.IsIdentifier() || (aTempToken.IsOperator() && aTempToken.GetId()==MULT_ID)) )
	{
		pNodeOut = 0;
		return true;
	}
	*/
	minClassBlockNode * pClassCode = 0;
	if( !ParseClassBlock( /*class-name*/sClassNameOut, pClassCode, bIsStruct ) )
	{
		pNodeOut = 0;
		SetError( ERROR_IN_CLASS, -1 );
		return false;
	}

    minClassDeclarationNode * pTemp = new minClassDeclarationNode( sClassNameOut, pClassCode, aBaseClassList, 0, minDebuggerInfo::CreateDebuggerInfo(aTempToken.GetLineNo()) );

	// die neue Struktur oder Klasse als Typ registrieren:
	RegisterUserType( pTemp );

	pNodeOut = pTemp;
	return true;
}

//*************************************************************************

static minOperatorNode * CreateNewOperatorNode( const minToken & aToken, bool bIsUnaray )
{
	switch( aToken.GetId() )
	{
		case EXISTS_ID :
                return new minExistsOperatorNode( aToken.GetString(), 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
#ifdef USEBIG
		case DEREFEXISTS_ID :
                return new minDereferenceExistsOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case DBGHALT_ID :
                return new minDebugHaltNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case TYPEOF_ID :
                return new minTypeofOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
#endif

		case INC_ID :
                return new minIncOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case DEC_ID :
                return new minDecOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );

		case NEW_ID :
                return new minNewOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case DELETE_ID :
                return new minDeleteOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case NOT_ID :
                return new minNotOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case INVERT_ID :
                return new minInvertOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case PTR_DEREF_ID :
                return new minPointerDereferenceOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case ADD_ID :
				if( bIsUnaray )
                    return new minPlusOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
				else
                    return new minAddOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case SUB_ID :
				if( bIsUnaray )
                    return new minMinusOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
				else
                    return new minSubOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case MULT_ID :
				if( bIsUnaray )
                    return new minDereferenceOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
				else
                    return new minMultOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case DIV_ID :
                return new minDivOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case MOD_ID :
                return new minModOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case LEFTSHIFT_ID :
                return new minShiftOperatorNode( aToken.GetString(), /*bLeftShift*/true, 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case RIGHTSHIFT_ID :
                return new minShiftOperatorNode( aToken.GetString(), /*bLeftShift*/false, 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case ASSIGN_ID :
                return new minAssignOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case LESSEQUAL_ID :
                return new minLessOperatorNode( aToken.GetString(), /*bLessEqual*/true, 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case LESS_ID :
                return new minLessOperatorNode( aToken.GetString(), /*bLessEqual*/false, 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case MOREEQUAL_ID :
                return new minMoreOperatorNode( aToken.GetString(), /*bMoreEqual*/true, 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case MORE_ID :
                return new minMoreOperatorNode( aToken.GetString(), /*bMoreEqual*/false, 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case EQUAL_ID :
                return new minEqualOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case NOT_EQUAL_ID :
                return new minNotEqualOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case LOG_AND_ID :
                return new minLogAndOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case LOG_OR_ID :
                return new minLogOrOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case BINARY_AND_ID :
				if( bIsUnaray )
                    return new minAddressOperatorNode( aToken.GetString(), minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
				else
                    return new minBinAndOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case BINARY_OR_ID :
                return new minBinOrOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case COMMA_ID :
                return new minCommaOperatorNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case DOT_ID :
                return new minObjectElementNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
		case ARRAY_START_ID :
                return new minArrayElementNode( aToken.GetString(), 0, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
	}
	return 0;
}

// Konstruktor-Aufruf implementieren (23.12.1999)
inline minInterpreterNode * MakeConstructorCallNode( const string & sTypeName, const minParserItemList & aArgumentList, const minToken & aToken )
{
    minFunctionCallNode * pFcnNode = new minFunctionCallNode( /*Variablename==Klassennamen*/sTypeName, aArgumentList, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
	return pFcnNode;
		// Bem.: beim Interpretieren des Calls wird automatisch der Name in einen Methodennamen konvertiert !!!
}

inline minInterpreterNode * MakeInitCallNode( minParserItemList & aArgumentListInOut )
{
	// bei einfachen Datentypen werden mehrere Argumente an new als Komma-Operator interpretiert,
	// d.h. der letzte Ausdruck gewinnt !  int * _pi = new int( 1, 2 ,3 ); --> *_pi == 3;
	// Hole das letzte Element aus der Argument-Liste und verwende dieses als Init-Ausdruck
	if( aArgumentListInOut.size()>0 )
	{
		minHandle<minInterpreterNode> & hInitNode = aArgumentListInOut.back();
		minInterpreterNode * pInitNode = hInitNode.GetPtr();
		// die Eigentuemerschaft an dem Node geht nun an den Aufrufer ueber !
		hInitNode.SetOwnerModus( false );
		return pInitNode;
	}
	return 0;
}


// parse eine Variablen-Deklaration (z.B. int i; double d = 1.0;) oder eine
// Funktions-Deklaration (z.B. int f( int i ) {... })
// oder ein Destruktor (z.B. ~MyClass() { ... })
// oder ein Konstruktor (z.B. MyClass( int i ) { ... })
// Moegliche Zenarien:
// Variablen-Deklaration: int i=0,j;
//						  int i,j=0;
//						  int i;
//						  int i,j;
//						  int i[8];
//						  int i[8],j[9];
//						  int i,aArr[10],bArr[8];
//						  MyClass aObj;
//						  MyClass aObj( 4 );
//						  int (*pFcn)();			// Funktionszeiger
//						  MyClass<int> aObj			// Templates, neu seit 14.2.2003
bool minParser::ParseVarDeclarationOrFunction( minInterpreterNode * & pNodeOut, bool bIsConst, bool bIsVirtual, const string * psClassName, bool bIsDestructor )
{
	// auf jeden Fall den Output-Parameter initialisieren
	pNodeOut = 0;

	StringListT aTemplateTypeNameList;

	// gepeektes (Typ) Token inkl. * und & Tokens komplett lesen
	minInterpreterType aIpType = ReadAndSubstitueTypeToken( bIsConst, psClassName, aTemplateTypeNameList );

	// ist es ein Template ?
	if( ParseForStringToken( "<" ) )
	{
		// ja --> dann die echten Typen lesen

		minToken aToken;

		while( PeekRealToken( aToken ) && (aToken.IsIdentifier() || aToken.IsKeyword()) )
		{
			ReadToken();

			aTemplateTypeNameList.push_back( aToken.GetString() );

			// TODO --> hier ggf. noch mal Pointer behandeln !
			// list<int *>

			// kommt ein Komma ?
			if( PeekRealToken( aToken ) && (aToken.GetId() == COMMA_ID) )
			{
				ReadToken();
			}
		}

		if( !ParseForStringToken( ">" ) )
		{
			pNodeOut = 0;
			return false;
		}
	}

	// ist es ein Konstruktor ? neu seit 19.12.1999
	bool bIsConstructor = false;
	if( (psClassName && (*psClassName==aIpType.GetTypeString())) )		// ist es ggf. eine Konstruktor-Deklaration ?
	{
		minToken aTestToken;
		// nach dem Klassen-Namen kann ein Funktionscall kommen: MyClass( int i ) ...
		if( PeekRealToken( aTestToken ) && aTestToken.IsParenthisOpen() )
		{
			bIsConstructor = true;
		}
	}

	minToken aToken;
	// TODO: ggf. Behandlung/Parsen von Funktionszeigern auslagern in Funktion...
	// ist es ggf. ein Funktionszeiger ? Beispiel: int (*pFcn)(); (4.1.2000)
	if( !bIsConstructor && !bIsDestructor && PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
	{
		// JA --> Funktionszeiger behandeln

		// gepeektes ( Token lesen
		m_pTokenizer->ReadNextToken();

		// jetzt muss ein * Token folgen
		if( PeekRealToken( aToken ) && aToken.IsOperator() && aToken.GetId()==MULT_ID )
		{
			// gepeektes * Token lesen
			m_pTokenizer->ReadNextToken();

			// jetzt folgt ein Identifier
			if( PeekRealToken( aToken ) && aToken.IsIdentifier() )
			{
				// gepeektes Identifier Token lesen
				m_pTokenizer->ReadNextToken();
				string sVariableName = aToken.GetString();

				// jetzt ist ein ) Token gefragt
				if( PeekRealToken( aToken ) && aToken.IsParenthisClose() )
				{
					// gepeektes ) Token lesen
					m_pTokenizer->ReadNextToken();

					// dann muss eine Parameter-Deklaration folgen
					minVariableDeclarationList aVarDeclList;
					if( ParseArgumentDeclarationList( aVarDeclList ) )
					{
						minInterpreterType aIpType = minInterpreterType( Function );
						minVariableDeclarationNode * pVarNode
							= new minVariableDeclarationNode(sVariableName, aIpType, /*nArraySize*/-1, 0, 0, 0, aTemplateTypeNameList, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()));

						pNodeOut = pVarNode;
						return true;
					}

					// und nun folgt ein Statement-Terminator...
					if( PeekRealToken( aToken ) && aToken.IsStatementTerminator() )
					{
						m_pTokenizer->ReadNextToken();
					}
				}
			}
		}

		SetError( PARSER_ERROR_IN_FCN_PTR_DECLARATION, aToken.GetLineNo() );
		return false;
	}

	// jetzt MUSS ein Identifier folgen, es sei denn es ist ein Konstruktor (19.12.1999)...
	int nArraySize = -1;		// bedeutet kein Array !
	if( bIsConstructor || bIsDestructor || /*bIsIdentifier*/(PeekRealToken( aToken ) && aToken.IsIdentifier()) )
	{
		string sVariableName;		// ist Variablennamen, Funktionsname oder Klassenname (Konstruktor)
		if( bIsConstructor || bIsDestructor )
		{
			sVariableName = aIpType.GetTypeString();
			// hier findet die Unterscheidung zwischen Konstruktor und Destruktor statt:
			// ein Destruktor ist praktisch genau so zu behandeln wie ein Konstruktor, er darf
			// jedoch keine Argumente besitzten und nur einmal vorkommen !
			if( bIsDestructor )
			{
				sVariableName = string("~")+sVariableName;
			}
		}
		else
		{
			sVariableName = aToken.GetString();
			// gepeektes (identifier) Token lesen, nur wenn es KEIN Konstruktor ist !
			m_pTokenizer->ReadNextToken();
		}

		// jetzt kann Zuweisung (=) oder , oder ; kommen --> dann ist es eine Variablen-Deklaration
		// oder ein ( --> dann ist es eine Funktions-Deklaration oder ein Konstruktor-Aufruf)
		if( PeekRealToken( aToken ) &&
			(aToken.IsStatementTerminator() || aToken.IsOperator() || aToken.IsParenthisOpen()) )
		{
			//=====================================================================
			//### Funktions-Deklaration bzw. Konstruktor/Destruktoraufruf implementieren
			//
			// Beispiel: int f( int x ) {...}; MyClass aObj( 4 );
			// ACHTUNG: keine Unterscheidung zwischen MyClass aObj(); als Funktions-Prototyp oder Variablen-Deklaration moeglich !

			if( aToken.IsParenthisOpen() )
			{
				// dann ist es eine Funktions-Deklaration,
				// oder die Deklaration eines Objektes, d.h. ein Konstruktor-Aufruf (19.12.1999)

				if( aIpType.IsReference() )
				{
					// ACHTUNG: Referenzen koennen noch nicht zurueckgegeben werden
					SetError( CANT_RETURN_REFERENCE, aToken.GetLineNo() );
					return false;
				}

				// Jetzt die Unterscheidung zwischen Funktionsdeklaration, Funktionsprototyp oder
				// Objekt-Instanzierung durchfuehren.
				// es ist nur dann eine Objekt-Instanzierung (d.h. Konstruktor Aufruf), wenn der
				// Typ eine Klasse ist und das erste Argument nach dem Call-Operator ( KEIN
				// gueltiges Typ-Token und KEIN ) ist.
				bool bIsObjectInstanceCall = false;
				if( aIpType.GetType()==Object )
				{
					// fuer die folgende Unterscheidung muss in den (...) Ausdruck herein gepeeked werden,
					// d.h. zwei Tokens voraus peeken, daher wird der Zustand des Tokenizers hier gesichert
					// um nach der Unterscheidung wieder restauriert zu werden !
					minTokenizerState * pActTokenizerState = m_pTokenizer->SaveState();

					ReadToken();

					if( PeekRealToken( aToken ) && !IsTokenValidType( aToken ) &&
						!aToken.IsParenthisClose() && !(aToken.IsKeyword() && aToken.GetId()==CONST_ID) )
					{
						bIsObjectInstanceCall = true;
					}

					m_pTokenizer->RestoreState( pActTokenizerState );
				}

				if( bIsObjectInstanceCall )
				{
					// *** es ist eine Deklaration eines Objektes ***

					// beim Parsen der Argumentliste wird auch das ( gelesen und auch das schliessende ) gelesen
					minParserItemList aArgumentList;
					if( ParseArgumentList( aArgumentList ) )
					{
						// jetzt ; lesen
						if( PeekRealToken( aToken ) && aToken.IsStatementTerminator() )
						{
							// gepeektes (;) Token lesen
							m_pTokenizer->ReadNextToken();

							// Variable anlegen (inkl. Konstruktor-Call),
							// ggf. ist dies ein const-Objekt, wenn vor dem Aufruf dieser Methode das const-Token detektiert wurde !
							minVariableDeclarationNode * pVarNode
                                = new minVariableDeclarationNode( sVariableName, aIpType, nArraySize, /*pArraySizeExpression*/0, /*pInitExpression*/0, MakeConstructorCallNode( aIpType.GetTypeString(), aArgumentList, aToken ), aTemplateTypeNameList, minDebuggerInfo::CreateDebuggerInfo( aToken.GetLineNo() ) );

							pNodeOut = pVarNode;
							return true;
						}
					}

					SetError( PARSER_ERROR_IN_OBJ_DECLARATION, aToken.GetLineNo() );
					return false;
				}
				else
				{
					// *** es ist eine Deklaration einer Funktion oder eines Konstruktors/Destruktors ***

					// jetzt die formalen Parameter der Funktion parsen:
					// dabei wird auch das ( gelesen und auch das schliessende ) gelesen
					minVariableDeclarationList aVarDeclList;
					minParserItemList aInitList;	// Initialisierungsliste, falls es ein Konstruktor ist (22.1.2000)
					// ACHTUNG: hier werden neue Objekte erzeugt und in den Kontainer als Zeiger hinzugefuegt !
					//			d.h. der Kontainer ist Eigentuemer an den neuen Objekten !
					if( ParseArgumentDeclarationList( aVarDeclList ) )
					{
						bool bIsConstMethod = false;
						// jetzt kann noch ein const-Token bei Methoden folgen !
						if( PeekRealToken( aToken ) && aToken.IsKeyword() && aToken.GetId()==CONST_ID )
						{
							ReadToken();
							bIsConstMethod = true;
						}
						// neu seit 22.1.2000
						// oder eine Konstruktor- bzw. Member-Initialisierungs Liste folgen
						// z.B. MyClass( int i ) : m_i( i ), MyBaseClass( i ) { ... }
						if( PeekRealToken( aToken ) && aToken.GetId()==DOT_DOT_ID )
						{
							ReadToken();

							if( !ParseConstructorInitList( sVariableName, aInitList ) )
							{
								SetError( PARSER_ERROR_IN_CONSTRUCTOR_INIT, aToken.GetLineNo() );
								return false;
							}
						}

						minInterpreterNode * pCode = 0;
						// hier sind auch Funktions-Prototypen erlaubt !
						if( ParseBlock( pCode, /*bNoBlockIsError*/false ) )
						{
							// ein Destruktor darf keine Parameter besitzen !
							if( bIsDestructor && aVarDeclList.size()>0 )
							{
								SetError( PARSER_ERROR_DESTRUCTOR_WITH_ARGUMENTS, aToken.GetLineNo() );
								return false;
							}

							// TODO: ggf. den Parameter bIsConst (z.B. bei const int f() {})
							//		 noch an den FunctionDecl.-Node uebergeben
                            pNodeOut = new minInterpreterFunctionDeclarationNode( sVariableName, bIsConstMethod, bIsVirtual, aIpType, aVarDeclList, pCode, aInitList, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
							return true;
						}
						else
						{
							SetError( PARSER_ERROR_IN_FCN_DECLARATION, aToken.GetLineNo() );	// oder in Konstruktor Deklaration !
							return false;
						}
					}
					else
					{
						SetError( PARSER_ERROR_IN_FCN_DECLARATION, aToken.GetLineNo() );	// oder in Konstruktor Deklaration !
						return false;
					}
				}
			}

			//=====================================================================
			//### Variable anlegen implementieren

			// bis hierhin ist aus "int i=0,j;" mindestens bis "int i" geparsed, im aToken steht "="
			// oder            aus "int i,j=0;" mindestens bis "int i" geparsed, im aToken steht ","
			// oder            aus "int i;"     mindestens bis "int i" geparsed, im aToken steht ";"
			// oder			   aus "int i[8];"	mindestens bis "int i" geparsed, im aToken steht "["
			// oder			   aus "MyClass aObj;"	mindestens bis "MyClass aObj" geparsed, im aToken steht ";"
			bool bContinue = true;
			bool bInitAllowed = true;
			minInterpreterNode * pNextNode = 0;
			minInterpreterNode * pArraySizeExpression = 0;
			// Schleife fuer mehrfach Variablen-Deklarationen
			while( bContinue )
			{
				minInterpreterNode * pInitExpression = 0;

				// Array-Deklaration behandeln (neu seit 7.12.1999)
				if( aToken.IsOperator() && aToken.GetId()==ARRAY_START_ID )
				{
					// gepeektes ([) Token lesen
					m_pTokenizer->ReadNextToken();
					// und dann die Expression parsen, es muss eine einfache Expression sein !

					// vorlaeufig nur 1 dimensionale Arrays zulassen !!!					
					assert( pArraySizeExpression == 0 );

					if( !ParseExpression( pArraySizeExpression, /*bStopWithComma*/true, /*bStopWithSemicolon*/true ) )
					{
						// Fehler in der Expression !
						SetError( ERROR_IN_EXPRESSION, aToken.GetLineNo() );
						return false;
					}

					// jetzt noch das Array-Stop Token lesen
					if( PeekRealToken( aToken ) && aToken.IsArrayStop() )
					{
						ReadToken();
					}
					else
					{
						SetError( ARRAY_STOP_EXPECTED, aToken.GetLineNo() );
						return false;
					}

					nArraySize = 0;
					// Array-Size muss ein zur Parse-Zeit konstanter Ausdurck sein
					if( pArraySizeExpression )
					{
						// eval zur parse-Zeit durchfuehren um die Array-Groesse zu bestimmen !
						// falls dieses scheitert, die Expression merken und an den VariableNode
						// weiterreichen (7.1.2003)
						try {
							minInterpreterValue aSizeVal;
							minInterpreterEnvironment aEnv;
							aEnv.SetSilentMode( true );
							if( pArraySizeExpression->Execute( /*bGetLValue*/false, aSizeVal, aEnv ) )
							{
								nArraySize = (int)aSizeVal.GetInt();
							}
							delete pArraySizeExpression;
							pArraySizeExpression = 0;
						}
						catch( ... )
						{
							nArraySize = UNKNOWN_ARRAY_SIZE;
						}
					}

					// Array-Deklarationen duerfen nicht initialisiert werden
					bInitAllowed = false;

					// mit der Schleife weiter machen, fuer multidimensionale Arrays wie: int aArr[10][10]
// ACHTUNG: bei multidimensionalen Arrays muss ggf. der Typ geaendert werden...
// Besser: Konzept fuer Arrays/Pointer? ueberlegen!!!
					PeekRealToken( aToken );
					continue;
				}

				// Variablen-Deklaration mit gleichzeitiger Initialisierung behandeln,
				if( aToken.IsOperator() && aToken.GetId()==ASSIGN_ID )
				{
					if( bInitAllowed )
					{
						// gepeektes (=) Token lesen
						m_pTokenizer->ReadNextToken();
						// und dann die Expression parsen
						if( !ParseExpression( pInitExpression, /*bStopWithComma*/true, /*bStopWithSemicolon*/true ) )
						{
							// Fehler in der Expression !
							SetError( ERROR_IN_EXPRESSION, aToken.GetLineNo() );
							return false;
						}
					}
					else
					{
						SetError( INITIALIZION_NOT_ALLOWED, aToken.GetLineNo() );
						return false;
					}
				}

				// Reference-Variablen muessen eine Zuweisung haben !
				if( aIpType.IsReference() && !pInitExpression )
				{
					SetError( REFERENCE_NEEDS_LVALUE, aToken.GetLineNo() );
					return false;
				}

				// WICHTIG: Beim Anlegen eines Objektes muss automatisch der (Default-)Destruktor aufgerufen werden !
				minInterpreterNode * pConstructorCall = 0;
				if( aIpType.GetType() == Object )
				{
                    pConstructorCall = MakeConstructorCallNode( aIpType.GetTypeString(), /*leere ArgumentList*/minParserItemList(), aToken );
				}
				// jetzt die Variable anlegen
				minVariableDeclarationNode * pNode
					= new minVariableDeclarationNode(sVariableName, aIpType, nArraySize, pArraySizeExpression, pInitExpression, pConstructorCall, aTemplateTypeNameList, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()));
				// gab es schon eine Variablen-Deklaration ?
				if( pNextNode )
				{
					((minVariableDeclarationNode *)pNextNode)->SetNextDeclaration( pNode );
				}
				pNextNode = pNode;
				if( !pNodeOut )
				{
					pNodeOut = pNode;
				}

				// und geht es weiter ?
				// hier erwartet man ein , oder ein ;
				if( PeekRealToken( aToken ) )
				{
					if( aToken.IsStatementTerminator() )
					{
						bContinue = false;
						// gepeektes (;) Token lesen
						m_pTokenizer->ReadNextToken();
					}
					// Mehrfach Variablen-Deklaration behandeln
					else if( aToken.IsOperator() && aToken.GetId()==COMMA_ID )
					{
						// gepeektes (,) Token lesen
						m_pTokenizer->ReadNextToken();

						// nach einer Varaibalen-Deklarationen mit Komma darf die naechste Varible wieder initialisiert werden
						bInitAllowed = true;

						// es geht weiter, Identifier einlesen
						if( PeekRealToken( aToken ) && aToken.IsIdentifier() )
						{
							// gepeektes (Identifer) Token lesen
							m_pTokenizer->ReadNextToken();
							sVariableName = aToken.GetString();
							// Typ ist schon weiter oben bestimmt worden !

							// jetzt weiter lesen, es kann ein = oder , oder ; kommen
							if( !PeekRealToken( aToken ) )
							{
								SetError( ERROR_READING_TOKEN, aToken.GetLineNo() );
								return false;
							}
						}
						else
						{
							SetError( EXPECT_IDENTIFIER, aToken.GetLineNo() );
							return false;
						}
					}
					else
					{
						// ungeultigen Operator gefunden
						SetError( UNEXPECTED_TOKEN, aToken.GetLineNo() );
						return false;
					}
				}
				else
				{
					SetError( ERROR_READING_TOKEN, aToken.GetLineNo() );
					return false;
				}
				nArraySize = -1;	// Array-Size wieder initialisieren ! (Bugfix 14.12.1999)
			}	// while-Schleife fuer mehrfach Variablen-Deklarationen

		}	// if( Operator-Abfrage )

		return true;

	}	// if( bIsConstructor || bIsIdentifier )

	SetError( PARSER_ERROR_IN_VAR_DECLARATION, aToken.GetLineNo() );
	return false;
}

bool minParser::ParseKeyword( const minToken & aPeekedToken, minInterpreterNode * & pNodeOut, const string * psClassName )
{
	int nId = aPeekedToken.GetId();
	int nLineNo = aPeekedToken.GetLineNo();
	if( nId == TYPEDEF_ID )
	{
		return ParseTypedef( pNodeOut );
	}
	else if( nId == WHILE_ID )
	{
		return ParseWhile( pNodeOut );
	}
	else if( nId == DO_ID )
	{
		return ParseDo( pNodeOut );
	}
	else if( nId == FOR_ID )
	{
		return ParseFor( pNodeOut );
	}
	else if( nId == IF_ID )
	{
		return ParseIf( pNodeOut );
	}
	else if( nId == SWITCH_ID )
	{
		return ParseSwitch( pNodeOut );
	}
	else if( nId == RETURN_ID )
	{
		// gepeektes (return) Token lesen
		m_pTokenizer->ReadNextToken();

		// folgt nun ein Ausdruck oder direkt ein ;
		minToken aToken;
		if( PeekRealToken( aToken ) && !aToken.IsStatementTerminator() )
		{
			minInterpreterNode * pExpression = 0;
			if( ParseExpression( pExpression ) )
			{
                pNodeOut = new minReturnNode( pExpression, minDebuggerInfo::CreateDebuggerInfo( aToken.GetLineNo() ) );
				return true;
			}
		}
		else if( aToken.IsStatementTerminator() )
		{
			// gepeektes (;) Token lesen
			m_pTokenizer->ReadNextToken();
            pNodeOut = new minReturnNode( 0, minDebuggerInfo::CreateDebuggerInfo( aToken.GetLineNo() ) );
			return true;
		}
		else
		{
			pNodeOut = 0;
			SetError( ERROR_IN_RETURN, aToken.GetLineNo() );
			return false;
		}
	}
	else if( nId == BREAK_ID )
	{
		// gepeektes (break) Token lesen
        minToken aBreakToken;
        m_pTokenizer->GetNextToken( aBreakToken );

		if( !ParseForStringToken( ";" ) )
		{
			pNodeOut = 0;
			SetError( EXPECTED_SEPARATOR, aBreakToken.GetLineNo() );
			return false;
		}

        pNodeOut = new minBreakNode( minDebuggerInfo::CreateDebuggerInfo( aBreakToken.GetLineNo() ));
		return true;
	}
	else if( nId == CONTINUE_ID )
	{
		// gepeektes (continue) Token lesen
        minToken aContinueToken;
        m_pTokenizer->GetNextToken( aContinueToken );

		if( !ParseForStringToken( ";" ) )
		{
			pNodeOut = 0;
			SetError( EXPECTED_SEPARATOR, aContinueToken.GetLineNo() );
			return false;
		}

        pNodeOut = new minContinueNode( minDebuggerInfo::CreateDebuggerInfo( aContinueToken.GetLineNo() ));
		return true;
	}
	else if( (nId == CLASS_ID) || (nId == STRUCT_ID) )
	{
		// gepeektes (class) Token lesen
		m_pTokenizer->ReadNextToken();

		string sClassName;
		return ParseClass( /*bIsStruct*/nId==STRUCT_ID, pNodeOut, sClassName );
	}
	else if( nId == CONST_ID )
	{
		// gepeektes (continue) Token lesen
		m_pTokenizer->ReadNextToken();

		// dann muss es eine Variable oder Funktion sein !
		return ParseVarDeclarationOrFunction( pNodeOut, /*bIsConst*/true, /*bIsVirtual*/false );
	}	
	else if( nId == VIRTUAL_ID )				// Bugfix 30.1.2003
	{
		// gepeektes (continue) Token lesen
		m_pTokenizer->ReadNextToken();

		// Bugfix 5.2.2003: es koennte ein virtueller Destruktor sein
		bool bIsDestructor = false;
		minToken aToken;
		if( PeekRealToken( aToken ) && (aToken.GetId()==INVERT_ID) )
		{
			bIsDestructor = true;
			m_pTokenizer->ReadNextToken();
		}

		// dann muss es eine Variable oder Funktion sein !
		return ParseVarDeclarationOrFunction( pNodeOut, /*bIsConst*/false, /*bIsVirtual*/true, psClassName, bIsDestructor );
	}
	else if( nId == THIS_ID )		// neu seit 9.1.2003
	{
		// this wie eine Expression behandeln !
		return ParseExpression( pNodeOut );
	}
	else if( nId == TEMPLATE_ID )		// neu seit 14.2.2003
	{
		// this wie eine Expression behandeln !
		return ParseTemplate( pNodeOut );
	}
	// ist es eine Variablen Deklaration ?
	else if( IsTokenValidType( aPeekedToken ) )
	{
		return ParseVarDeclarationOrFunction( pNodeOut, /*bIsConst*/false, /*bIsVirtual*/false );
	}
	// Bugfix 6.2.2003: fuer case: labels
	else if( (nId == CASE_ID) || (nId == DEFAULT_ID) )
	{
		pNodeOut = 0;
		return false;
	}

	pNodeOut = 0;
	SetError( UNEXPECTED_KEYWORD, nLineNo );
	return false;
}

bool minParser::ParseSizeof( minInterpreterNode * & pNodeOut )
{
	// gepeektes (sizeof) Token lesen 
    minToken aSizeofToken;
    m_pTokenizer->GetNextToken( aSizeofToken );

	// Bedingung in Klammern parsen
	minInterpreterNode * pExpression = 0;
	if( ParseForStringToken( "(" ) && 
		ParseExpression( pExpression, /*bStopWithComma*/false, /*bStopWithSemicolon*/true ) &&
		ParseForStringToken( ")" ) )
	{
        pNodeOut = new minSizeofNode( pExpression, minDebuggerInfo::CreateDebuggerInfo( aSizeofToken.GetLineNo() ) );
		return true;
	}
	pNodeOut = 0;
	SetError( PARSER_ERROR_IN_SIZEOF, aSizeofToken.GetLineNo() );
	return false;
}

bool minParser::ParseTemplate( minInterpreterNode * & pNodeOut )
{
	// parse: template <class Type1, class Type2> class/function

	SkipWhitespaces();

	// gepeektes (sizeof) Token lesen 
    minToken aToken;
    m_pTokenizer->GetNextToken(aToken);

	minInterpreterNode * pClass = 0;
	string sClassName;

	if( ParseTemplateArgumentList(m_aTemplateTypeNameList) )
	{
		// process template classes
		if( ParseForStringToken("class") &&
			ParseClass( /*bIsStruct*/false, pClass, sClassName) )
		{
			pNodeOut = new minTemplateNode(m_aTemplateTypeNameList, pClass, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()));

			// temporaere Type Liste wieder loeschen
			m_aTemplateTypeNameList.erase(m_aTemplateTypeNameList.begin(), m_aTemplateTypeNameList.end());

			return true;
		}

		// process template functions
		if( ParseVarDeclarationOrFunction(pNodeOut, /*bIsConst*/false, /*bIsVirtual*/false) )
		{
// TODO --> Template Functions realisieren !
			SetError( PARSER_ERROR_IN_TEMPLATE_NO_FUNCTIONS_SUPPORTED, aToken.GetLineNo() );
			return false;
		}
	}

	pNodeOut = 0;
	SetError( PARSER_ERROR_IN_TEMPLATE, aToken.GetLineNo() );
	return false;
}

bool minParser::ParseTemplateArgumentList( StringListT & aTypeNameListOut )
{
	// parse: <class Identifier1, class Identifier2, ... >
	// Die Liste kann auch leer sein !

	// Ausgabeliste erst mal loeschen
	aTypeNameListOut.erase( aTypeNameListOut.begin(), aTypeNameListOut.end() );

	// ist es ein Template ?
	if( ParseForStringToken( "<" ) )
	{
		minToken aToken;

		while( ParseForStringToken( "class" ) )
		{
			if( PeekRealToken( aToken ) && aToken.IsIdentifier() )
			{
				ReadToken();
				aTypeNameListOut.push_back( aToken.GetString() );
			}

			// kommt ein Komma ?
			if( PeekRealToken( aToken ) && (aToken.GetId() == COMMA_ID) )
			{
				ReadToken();
			}
		}

		if( ParseForStringToken( ">" ) )
		{
			return true;
		}
	}

	SetError( PARSER_ERROR_IN_TEMPLATE_ARGS, -1 );
	return false;
}

bool minParser::RegisterTypedefItem( const minInterpreterType & aOldType, const string & sNewTypeString )
{
	// gibt es den neuen Typnamen schon ?
	if( IsTypedefType( sNewTypeString ) )
		return false;

	// alter Typname == neuer Typname, einfach ignorieren ! (Bugfix: Endlosloop bei typedef class MyClass {} MyClass; 4.1.2000)
	if( aOldType.IsObject() && aOldType.GetTypeString()==sNewTypeString )
		return true;

	m_aTypedefContainer.push_back( pair<minInterpreterType,string>( aOldType, sNewTypeString ) );
	return true;
}

bool minParser::IsTypedefType( const string & aTypeString ) const
{
	MyTypedefContainer::const_iterator aIter = m_aTypedefContainer.begin();

	while( aIter != m_aTypedefContainer.end() )
	{
		if( aTypeString == (*aIter).second )
			return true;
		++aIter;
	}

	return false;
}

bool minParser::GetTypedefSubstitution( const string & aTypeStringIn, minInterpreterType & aOrgTypeOut ) const
{
	MyTypedefContainer::const_iterator aIter = m_aTypedefContainer.begin();

	while( aIter != m_aTypedefContainer.end() )
	{
		if( aTypeStringIn == (*aIter).second )
		{
			aOrgTypeOut = (*aIter).first;
			return true;
		}
		++aIter;
	}

	return false;
}

bool minParser::RegisterUserType( const minClassDeclarationNode * pNewUserType )
{
	m_aUserTypeContainer.push_back( pNewUserType );
	return true;
}

bool minParser::IsTemplateType( const string & sName ) const
{
	return (find( m_aTemplateTypeNameList.begin(), m_aTemplateTypeNameList.end(), sName ) != m_aTemplateTypeNameList.end());
}

bool minParser::IsUserType( const string & sName ) const
{
	MyUserTypeContainer::const_iterator aIter = m_aUserTypeContainer.begin();

	while( aIter != m_aUserTypeContainer.end() )
	{
		if( (*aIter)->GetName() == sName )
			return true;
		++aIter;
	}

	return false;
}

bool minParser::IsUserTypeDefined( const string & sName ) const
{
	MyUserTypeContainer::const_iterator aIter = m_aUserTypeContainer.begin();

	while( aIter != m_aUserTypeContainer.end() )
	{
		if( ((*aIter)->GetName()==sName) && ((*aIter)->IsDefined()) )
			return true;
		++aIter;
	}

	return false;
}

bool minParser::IsBuildInType( const minToken & aPeekedToken ) const
{
	int nId = aPeekedToken.GetId();
	if( (nId==INT_ID) || (nId==LONG_ID) || (nId==SHORT_ID) || (nId==CHAR_ID) ||
		(nId==UINT_ID) || (nId==ULONG_ID) || (nId==USHORT_ID) || (nId==UCHAR_ID) ||
		(nId==FLOAT_ID) || (nId==DOUBLE_ID) ||
		(nId==STRING_ID) || (nId==BOOL_ID) || (nId==VOID_ID) )
		return true;
	return false;
}

bool minParser::IsTokenValidType( const minToken & aPeekedToken ) const
{
	//int nId = (int)aPeekedToken.GetId();
	if( IsBuildInType( aPeekedToken ) || 
		IsUserType( aPeekedToken.GetString() ) ||
		IsTemplateType( aPeekedToken.GetString() ) ||			// neu seit 14.2.2003
		IsTypedefType( aPeekedToken.GetString() )
#ifdef _with_object_keyword
        /*nur zum TEST*/ || (nId==OBJECT_ID)
#endif
        )
		return true;
	return false;
}

bool minParser::IsVariableScopeModifier( const minToken & aToken )
{
	if( aToken.IsKeyword() && (aToken.GetId()==STATIC_ID || aToken.GetId()==EXTERN_ID) )
		return true;
	return false;
}

bool minParser::IsUnaryOperator( const minToken & aToken ) const
{
	if( aToken.IsOperator() )
	{
		int nId = aToken.GetId();
		if( nId==NOT_ID || nId==INC_ID || nId==DEC_ID ||
			nId==ADD_ID || nId==SUB_ID ||						// neu seit 9.1.2003 fuer Konstanten wie -1 oder +1 !
			nId==MULT_ID || nId==NEW_ID || nId==DELETE_ID ||	// MULT_ID, NEW_ID und DELETE_ID neu seit 25.12.1999 fuer *pInt
			nId==BINARY_AND_ID ||								// neu seit 4.1.2000 fuer &i;
			nId==INVERT_ID )									// Bugfix 27.1.2003
			return true;
	}
	return false;
}

// Hilfsfunktion, zum Parsen der Argument-Liste eines Funktionsaufrufes
// (z.B. f(1,2); }.
// Vorbedingung: das erste Token ( darf noch nicht aus dem Tokenizer gelesen sein !
// Nachbedingung: die gesamte Klammer, inklusive des schliessenden ) wird gelesen,
//				  es wird true geliefert wenn alles ok, ansonsten false !
bool minParser::ParseArgumentList( minParserItemList & aArgumentList )
{
	// parse eine Liste von Expressions, die durch ein Komma getrennt sind
	// und die mit einem ) (oder einem Fehler) beendet wird
	minToken aToken;
	bool bContinue = true;

	// es muss noch das ( gelesen werden
	if( PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
	{
		ReadToken();

		// ist das Lesen von ( erfolgreich, dann weiter machen
		while( bContinue && PeekRealToken( aToken ) )
		{
			// es kann eine Expression oder ein ) kommen
			if( aToken.IsParenthisClose() )
			{
				bContinue = false;
				ReadToken();		// lese auch abschliessendes )
			}
			else
			{
				// es ist wohl eine Expression (d.h. quasi einen rekursiven Aufruf durchuehren !)
				minInterpreterNode * pExpression = 0;
				bContinue = ParseExpression( pExpression, /*bStopWithComma*/true );

				aArgumentList.push_back( minHandle<minInterpreterNode>( pExpression ) );

				// jetzt koennte ein , oder ein ) folgen
				if( PeekRealToken( aToken ) && (aToken.GetId()==COMMA_ID) )
				{
					// Komma gefunden es geht einfach weiter
					// gepeektes (Identifier) Token auch lesen
					ReadToken();
				}
				// das ) wird im naechsten Schleifendurchlauf behandelt !
			}

		}

		// trat ein Fehler auf oder ist die Argument-Liste des Funktionsaufrufs einfach (korrekt) beendet ?
		return aToken.IsParenthisClose();
	}

	return false;
}

// bestimme den linken Expression-Baum, dessen Operator-Level groesser/gleich 
// dem uebergebenen Operator-Level ist. Es kann auch 0 geliefert werden !!!
inline minInterpreterNode * GetLeftestOperatorNode( minInterpreterNode * pExprNode, int iOperatorLevel )
{
	if( pExprNode && pExprNode->IsOperator() )
	{
		minInterpreterNode * pTempNode = ((minOperatorNode *)pExprNode)->GetLeftNode();

		if( pTempNode && pTempNode->IsOperator() && (pTempNode->GetOperatorLevel() >= iOperatorLevel) )
		{
			return GetLeftestOperatorNode( pTempNode, iOperatorLevel );
		}
		return pExprNode;
	}
	return 0;
}

// Funktions-Makro: wird an zwei Stellen in ParseExpression() verwendet
inline void SortForOperatorLevels( minInterpreterNode * pRecursiveExpr, minInterpreterNode * pNewOperator, minInterpreterNode * & pExpressionOut  )
{
	// Ergebnis des rekursiven Aufrufs verarbeiten
	if( pRecursiveExpr && pRecursiveExpr->IsOperator() )
	{
		if( pRecursiveExpr->GetOperatorLevel() >= pNewOperator->GetOperatorLevel() )
		{
			// Sortiere den neuen Operator-Baum an den Baum des rekursiven Aufrufs
			// an der passenden Stelle ein, Operatorlevel muss groesser oder gleich sein !
			minInterpreterNode * pLeftesOperatorItem = GetLeftestOperatorNode( pRecursiveExpr, pNewOperator->GetOperatorLevel() );
			minInterpreterNode * pLeftesItem = pLeftesOperatorItem ? ((minOperatorNode *)pLeftesOperatorItem)->GetLeftNode() : 0;

			((minOperatorNode *)pNewOperator)->SetRightNode( pLeftesItem );
			((minOperatorNode *)pLeftesOperatorItem)->SetLeftNode( pNewOperator );
			pExpressionOut = pRecursiveExpr;
		}
		else
		{
			((minOperatorNode *)pNewOperator)->SetRightNode( pRecursiveExpr );
			pExpressionOut = pNewOperator;
		}
	}
	else
	{
		// es ist ein einfacher Identifier/Konstante zurueckgekommen
		((minOperatorNode *)pNewOperator)->SetRightNode( pRecursiveExpr );
		pExpressionOut = pNewOperator;
	}
}

// behandelt auch ggf. vorkommende Escape-Zeichen, z.B. \n 
inline char GetCharFromString( const string & s )
{
	char ch = s[0];

	if( (s.length()>1) && (s[0]==g_chEscapeChar) )
	{
		string sTemp = minTokenizer::GetStringEscapeChar( s[1] );
		if( sTemp.length()>0 )
		{
			ch = sTemp[0];
		}
	}

	return ch;
}

bool minParser::ParseExpression( minInterpreterNode * & pExpressionOut, bool bStopWithComma, bool bStopWithSemicolon, bool bStopWithArrayStop )
{
	minToken aToken, aLastToken;

	if( PeekRealToken( aToken ) )
	{
		minInterpreterNode * pLeft = 0;
		minOperatorNode * pNewOperator = 0;

		// linke Seite verarbeiten
		if( aToken.IsKeyword() && aToken.GetId()==THIS_ID )
		{
			// das this wird wie ein identifier bearbeitet...

			ReadToken();

			pLeft = new minThisNode();
		}
		// neu seit 7.1.2003: Spezialbehandlung fuer sizeof() Operator
		else if( (aToken.GetId() == SIZEOF_ID) )
		{
			if( !ParseSizeof( pLeft ) )
			{
				pExpressionOut = 0;
				return false;
			}
		}
		else if( aToken.IsIdentifier() )
		{
			aLastToken = aToken;
			ReadToken();

//#ifdef USEBIG
			//*************************************************************
			// neu seit 5.1.2000
			// Spezialbehandlung von defined fuer den Preprocessor: (behandle define wie einen einfachen Identifier)
			// steht nur hier und koennte somit relativ einfach ueber ein Flag gesteuert werden.
			// Fall a) defined identifier oder Fall b) defined( identifier ) sind moeglich
			// es wird dann ein _*exists Operator erzeugt...
			if( aToken.GetString()=="defined" )
			{
				// Spezialfall eingetreten...
				bool bFirstCase = true;
				minToken aTempToken;
				if( PeekRealToken( aTempToken ) && aTempToken.IsParenthisOpen() )
				{
					// Fall b) ist eingetreten
					ReadToken();
					bFirstCase = false;
				}

				// Identifier Einlesen
				if( PeekRealToken( aTempToken ) /*&& aTempToken.IsIdentifier()*/ )
				{
					ReadToken();
                    pLeft = new minExistsOperatorNode( /*sOperator*/"__exists", new minVariableNode( aTempToken.GetString(), Unknown, minDebuggerInfo::CreateDebuggerInfo( aTempToken.GetLineNo() ) ), minDebuggerInfo::CreateDebuggerInfo( aTempToken.GetLineNo() ) );
				}
				//ggf. bei Identifier Ueberpruefeung else --> ERROR

				if( !bFirstCase )
				{
					// fuer den Fall b) noch das ) lesen
					if( PeekRealToken( aTempToken ) && aTempToken.IsParenthisClose() )
					{
						ReadToken();
					}
					else
					{
						SetError( ERROR_IN_DEFINED, aTempToken.GetLineNo() );
						return false;
					}
				}
			}
			//*************************************************************
			// ist es ein einfache Identifier, ein Call oder ein Ausdruck mit Array-Operator (aArr[1], neu seit 8.12.1999) ?
			else 
//#endif
				  if( PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
			{
				// es ist ein Funktionsaufruf !

				// d.h. jetzt noch die Argumente parsen und
				// wenn Argumente korrekt geparsed wurden Node anlegen
				minParserItemList aArgumentList;
				if( ParseArgumentList( aArgumentList ) )
				{
                    pLeft = new minFunctionCallNode( aLastToken.GetString(), aArgumentList, 0, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
				}
				else
				{
					SetError( ERROR_IN_FCN_CALL, aLastToken.GetLineNo() );
					return false;
				}
			}
			else if( PeekRealToken( aToken ) && aToken.IsArrayOperator() )
			{
				// es ist ein Array-Operator Ausdruck
				ReadToken();

				minInterpreterNode * pIndexExpr = 0;
				if( ParseExpression( pIndexExpr, /*bStopWithComma*/false, /*bStopWithSemicolon*/false, /*bStopWithArrayStop*/true ) )
				{
					// die Expression MUSS mit einem ] schliessen
					if( PeekRealToken( aToken ) && aToken.IsArrayStop() )
					{
						ReadToken();
                        pLeft = new minArrayElementNode( aToken.GetString(), new minVariableNode( aLastToken.GetString(), Unknown, minDebuggerInfo::CreateDebuggerInfo( aToken.GetLineNo() ) ), pIndexExpr, minDebuggerInfo::CreateDebuggerInfo( aLastToken.GetLineNo() ) );
// TODO: hier ggf. rekursiv aufsetzten...
					}
					else
					{
						SetError( ARRAY_STOP_EXPECTED, aToken.GetLineNo() );
						return false;
					}
				}
				else
				{
					SetError( ERROR_IN_ARRAY_INDEX, aToken.GetLineNo() );
					return false;
				}
			}
			else
			{
				// dann ist es wohl doch nur ein einfacher Identifier

				// bestimme erst den Typ und lege dann den Variablen-Knoten an
				//minInterpreterType aIpType;
				// Bem.: zur Zeit kann nicht zur Parse/Compile-Zeit ueberprueft
				//		 werden ob eine Variable schon existiert ! Dies geschieht
				//		 erst zur Laufzeit !!!
				if( true/*GetVariableInfo( aLastToken.GetString(), aIpType )*/ )
				{
					pLeft = new minVariableNode( aLastToken.GetString(), /*aIpType*/Unknown, minDebuggerInfo::CreateDebuggerInfo(aLastToken.GetLineNo()) );
				}
				//else
				//{
				//	// Variable ist nicht bekannt !!!
				//	SetError( UNKNOWN_VARIABLE );
				//	return false;
				//}
			}
		}
		else if( aToken.IsConstant() )
		{
			minInterpreterValue aVal;

			if( aToken.IsIntNumber() )
			{
				// hex constants since 27.1.2003
				if( aToken.IsHexConstant() )
				{
					aVal = minInterpreterValue( (int)strtol( aToken.GetString().c_str(), 0, 16 ) );
				}
				else
				{
					aVal = minInterpreterValue( atoi( aToken.GetString().c_str() ) );
				}
			}
			else if( aToken.IsFloatNumber() )
			{
				aVal = minInterpreterValue( atof( aToken.GetString().c_str() ) );
			}
			else if( aToken.IsChar() )
			{
				// Bugfix 27.1.2003
				aVal = minInterpreterValue( GetCharFromString( aToken.GetString() )/*aToken.GetString()[0]*//*.at( 0 )*/ );    // PALM
			}
			else if( aToken.IsBool() )
			{
				bool bVal = false;
				if( aToken.GetString()==string( _TRUE ) )
					bVal = true;
				aVal = minInterpreterValue( bVal );
			}
			else
			{
				aVal = minInterpreterValue( aToken.GetString() );
			}
			pLeft = new minConstantNode( aVal, minDebuggerInfo::CreateDebuggerInfo( aToken.GetLineNo() ) );
			ReadToken();
		}
		else if( aToken.IsOperator() )
		{
			// falls es ein , Operator ist ueberpruefen ob dies die Expression terminieren soll ?
			// wohl nicht notwendig !
			//if( bStopWithComma && (aToken.GetId()==COMMA_ID) )
			//{
			//	return false;
			//}

			// dies waere ein Vorzeichen... oder der ! Operator oder der * Operator (Derefenence)
			// d.h. ein pre Unary Operator !
			if( IsUnaryOperator( aToken ) )
			{
				ReadToken();
				pNewOperator = CreateNewOperatorNode( aToken, /*bIsUnaray*/true );

				// Bugfix 27.1.2003
				if( pNewOperator == 0 )
				{
					SetError( UNKNOWN_OPERATOR, aToken.GetLineNo() );
					return false;
				}

				// seit 25.12.1999: hier eine Spezialbehandlung des new-Operators durchfuehren
				if( aToken.GetId()==NEW_ID )
				{
					// Beispiele:	int p = new int;
					//				int p = new int [10];
					//				MyClass p = new MyClass(1,2.3);
					//				MyClass p = new MyClass[8];

					StringListT aTemplateTypeNameList;

					// gepeektes (Typ) Token inkl. * und & Tokens komplett lesen
					minInterpreterType aIpType = ReadAndSubstitueTypeToken( /*bIsConst*/false, 0, aTemplateTypeNameList );

					// es MUSSTE ein Typ-Token kommen !!!
					if( aIpType.IsValidType() && !aIpType.IsReference() )
					{
						// jetzt kann noch eine Initialisierung der neu erzeugen Variable erfolgen, z.B. int * p = new int( 8 );
						minParserItemList aArgumentList;			// leere Argument-Liste !
						if( ParseArgumentList( aArgumentList ) )
						{
							// es wurden noch Argumente an den New-Operator uebergeben
							// diese werden gleich im Konstruktor-Call-Node erzeugen verwendet !
						}

						((minNewOperatorNode *)pNewOperator)->SetType( aIpType );
						// falls es ein Objekt ist, einen Konstruktor-Aufruf im new durchfuehren
						if( aIpType.IsObject() )
						{
                            ((minNewOperatorNode *)pNewOperator)->SetConstructorCall( MakeConstructorCallNode( aIpType.GetTypeString(), aArgumentList, aToken ) );
						}
						else
						{
							// initialisiere einfachen Datentyp mit Assign-Operator
							((minNewOperatorNode *)pNewOperator)->SetConstructorCall( MakeInitCallNode( aArgumentList ) );
						}
						// seit 14.2.2003, fuer Template Behandlung
						((minNewOperatorNode *)pNewOperator)->SetAllTemplateTypes( aTemplateTypeNameList );
// TODO gulpxyz hier noch Arrays anlegen lassen
					}
					else
					{
						SetError( NEED_TYPE_FOR_NEW, aToken.GetLineNo() );
						return false;
					}
					pExpressionOut = pNewOperator;
					return true;
				}
				else
				{
					// rekursiver Aufruf
					minInterpreterNode * pRecursiveExpr = 0;
					if( ParseExpression( pRecursiveExpr, bStopWithComma, bStopWithSemicolon, bStopWithArrayStop ) )
					{
						// Ergebnis des rekursiven Aufrufs verarbeiten (Bugfix: 25.12.1999)
						SortForOperatorLevels( pRecursiveExpr, pNewOperator, pExpressionOut );
						return true;
					}
				}
			}
		}
		else if( aToken.IsParenthisOpen() )
		{
            minToken aParenthisToken = aToken;
			ReadToken();						// Klammer auf lesen
			if( ParseExpression( pLeft, bStopWithComma, bStopWithSemicolon, bStopWithArrayStop ) )		// rekursiver Aufruf
			{
				if( PeekRealToken( aToken ) && aToken.IsParenthisClose() )
				{
                    pLeft = new minParenthisNode( pLeft, minDebuggerInfo::CreateDebuggerInfo(aParenthisToken.GetLineNo()) );
					ReadToken();
				}
				else
				{
					SetError( EXPECT_PARENTHIS_CLOSE, aToken.GetLineNo() );
					return false;
				}
			}
			else
			{
				SetError( ERROR_IN_PARENTHIS, aToken.GetLineNo() );
				return false;
			}
		}
		else
		{
			SetError( EXPECT_IDENTIFIER_OR_CONSTANT, aToken.GetLineNo() );
			return false;
		}
		// wenn man hier ist, dann ist Token ok, jetzt lesen
		//old: ReadToken();
		//aLastToken = aToken;

		// neu seit 4.1.2000:
		// ist es ein Call-Operator ? Der kann auch nach einem Klammerasdurck folgen, z.B. (*pfcn)();
		if( PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
		{
			// es ist ein Funktionsaufruf !

			// d.h. jetzt noch die Argumente parsen und
			// wenn Argumente korrekt geparsed wurden Node anlegen
			minParserItemList aArgumentList;
			if( ParseArgumentList( aArgumentList ) )
			{
                pLeft = new minFunctionCallNode( aLastToken.GetString(), aArgumentList, pLeft, minDebuggerInfo::CreateDebuggerInfo(aToken.GetLineNo()) );
			}
			else
			{
				SetError( ERROR_IN_FCN_CALL, aToken.GetLineNo() );
				return false;
			}
		}

		// Bugfix 23.1.2003
		// Spezialfall der post inc und dec Operatoren behandeln (i++, i--)
		if( PeekRealToken( aToken ) && aToken.IsOperator() && 
			(aToken.GetId()==INC_ID || aToken.GetId()==DEC_ID) )
		{
			ReadToken();

			pNewOperator = CreateNewOperatorNode( aToken, /*bIsUnaray*/true );
			if( pNewOperator )
			{
				((minOperatorNode *)pNewOperator)->SetLeftNode( pLeft );

				pLeft = pNewOperator;
			}
			// behandle den Ausdruck i++ ab jetzt wie ein Identifier !
		}

		// Operator verarbeiten
		if( PeekRealToken( aToken ) && aToken.IsOperator() )
		{
			// falls es ein , Operator ist ueberpruefen ob dies die Expression terminieren soll ?
			if( bStopWithComma && (aToken.GetId()==COMMA_ID) )
			{
				pExpressionOut = pLeft;
				return true;
			}

			ReadToken();

			pNewOperator = CreateNewOperatorNode( aToken, /*bIsUnaray*/false );
			if( pNewOperator )
			{
				((minOperatorNode *)pNewOperator)->SetLeftNode( pLeft );

				// 'Spezialbehandlung' ist es ein Array-Start Operator ([) ?
				bool bIsArrayStart = (aToken.GetId()==ARRAY_START_ID);

				// rekursiver Aufruf fuer rechte Seite ?
				// zum weiterparsen einen rekursiver Aufruf absezten !!!
				minInterpreterNode * pRecursiveExpr = 0;
				if( ParseExpression( pRecursiveExpr, bStopWithComma, bStopWithSemicolon, /*bStopWithArrayStop*/bIsArrayStart ) )
				{
					// Ergebnis des rekursiven Aufrufs verarbeiten
					SortForOperatorLevels( pRecursiveExpr, pNewOperator, pExpressionOut );

					// 'Spezialbehandlung' falls es ein Array-Start war !
					if( bIsArrayStart )
					{
						// jetzt noch das Array-Stop Token lesen
						if( PeekRealToken( aToken ) && aToken.IsArrayStop() )
						{
							ReadToken();
						}
						else
						{
							SetError( ARRAY_STOP_EXPECTED, aToken.GetLineNo() );
							return false;
						}
					}

					return true;
				}
			}
			else
			{
				SetError( UNKNOWN_OPERATOR, aToken.GetLineNo() );
				return false;
			}
		}
		// ist es ein Satement-Ende-Zeichen
		else if( PeekRealToken( aToken ) && aToken.IsStatementTerminator() )
		{
			// soll das Statement-End Token auch gelesen werden ?
			if( !bStopWithSemicolon )		// neu seit 17.4.1999
			{
				ReadToken();
			}
			pExpressionOut = pLeft;
			return true;
		}
		// Bugfix 6.2.2003, fuer case 'a':
		else if( PeekRealToken( aToken ) && (aToken.GetId()==DOT_DOT_ID) )
		{
			pExpressionOut = pLeft;
			return true;
		}
		// ist es ein Array-Stop-Zeichen
		else if( PeekRealToken( aToken ) && aToken.IsArrayStop() )
		{
			// es soll mit einem Array-Stop Token gestopt werden, Token NICHT lesen ! --> siehe 'Spezialbehandlung' oben
		/*
			if( !bStopWithArrayStop )
			{
				ReadToken();
			}
		*/
			pExpressionOut = pLeft;
			return true;
		}
		// ist es ein Klammer-Zu-Zeichen
		else if( PeekRealToken( aToken ) && aToken.IsParenthisClose() )
		{
			// wichtig: bei schliessen der Klammer den Token NICHT lesen: ReadToken();
			pExpressionOut = pLeft;
			return true;
		}
	/* dies ist wohl nicht notwendig !
		// ist es ein Call-Separator-Zeichen (ACHTUNG: ist auch ein Komma-Operator!)
		else if( PeekRealToken( aToken ) && aToken.IsParenthisClose() )
		{
			// wichtig: bei schliessen der Klammer den Token NICHT lesen: ReadToken();
			pExpressionOut = pLeft;
			return true;
		}
	*/
	}

	pExpressionOut = 0;
	return false;
}

void minParser::HandleError( int nLineNo )
{
	cout << "Parser-Error occured !!!" << endl;
	cout << "  ErrorNo  = " << GetErrorCode() << endl;
	cout << "  ErrorPos = " << m_pTokenizer->GetErrorPos() << endl;
	cout << "  Line No  = " << nLineNo << endl;
}

void minParser::SetProgramNode( minInterpreterNode * pNewProgNode )
{
	if( m_pProgramNode )
		delete m_pProgramNode;
	m_pProgramNode = pNewProgNode;
}

bool minParser::PeekRealToken( minToken & aTokenOut )
{
	return m_pTokenizer->PeekRealToken( aTokenOut );
}

minInterpreterType minParser::ReadAndSubstitueTypeToken( bool bIsConst, const string * psActClassName, StringListT & aTemplateTypeNameListOut )
{
	minToken aToken;
	PeekRealToken( aToken );
	ReadToken();

	// jetzt noch in der Typedef-Liste nach Substitutionen suchen...
	// und zwar so lange, bis ein einfacher oder User-Type herauskommt,
	// d.h. hier werden auch mehrfache typedef-Statements ersetzt !
	string sTypeName = aToken.GetString();
	int nPointerLevelSum = 0;
	bool bIsReferenceSum = false;
	while( IsTypedefType( sTypeName ) )
	{
		minInterpreterType aTempType;
		if( GetTypedefSubstitution( sTypeName, aTempType ) )
		{
			sTypeName = aTempType.GetRealTypeString();
			// es koennen mehrfache Zeigerebenen vorkommen:
			// typedef int * PINT; typedef PINT * PPINT;
			nPointerLevelSum += aTempType.GetPointerLevel();
			bIsReferenceSum |= aTempType.IsReference();
		}
	}

	// ist es ein Template ?
	if( ParseForStringToken( "<" ) )
	{
		// ja --> dann die echten Typen lesen

		while( PeekRealToken( aToken ) && (aToken.IsIdentifier() || aToken.IsKeyword()) )
		{
			ReadToken();

			aTemplateTypeNameListOut.push_back( aToken.GetString() );

			// TODO --> hier ggf. noch mal Pointer behandeln !
			// list<int *>

			// kommt ein Komma ?
			if( PeekRealToken( aToken ) && (aToken.GetId() == COMMA_ID) )
			{
				ReadToken();
			}
		}

		if( !ParseForStringToken( ">" ) )
		{
			SetError( PARSER_ERROR_IN_TEMPLATE_ARGS, aToken.GetLineNo() );
			return minInterpreterType();
		}
	}

	// jetzt koennen noch beliebig viele *
	// und anschliessend ein & folgen
	int nPointerLevel = 0;
	bool bIsReference = false;
	GetRefAndPointerLevel( nPointerLevel, bIsReference );

	return minInterpreterType( GetTypeFromString( sTypeName, psActClassName ), nPointerLevelSum+nPointerLevel, bIsReferenceSum | bIsReference, /*Bugfix: bis 4.1.2000 aToken.GetString()*/sTypeName, bIsConst );
}

// Makro, da an mehreren Stellen verwendet
void minParser::GetRefAndPointerLevel( int & nPointerLevel, bool & bIsReference )
{
	minToken aToken;

	// jetzt kann ein Pointer-Operator (*) folgen
	nPointerLevel = 0;
	while( PeekRealToken( aToken ) && aToken.IsOperator() && (aToken.GetId()==MULT_ID) )
	{
		// gepeektes (Pointer) Token lesen
		m_pTokenizer->ReadNextToken();

		nPointerLevel++;
	}

	// und jetzt kann ein Referenz-Operator (&) folgen
	bIsReference = false;
	if( PeekRealToken( aToken ) && aToken.IsOperator() && (aToken.GetId()==BINARY_AND_ID) )
	{
		// gepeektes (Reference) Token lesen
		m_pTokenizer->ReadNextToken();

		bIsReference = true;
	}
}

InterpreterValueType minParser::GetTypeFromString( const string & aTypeString, const string * psActClassName ) const
{
	if( IsUserType( aTypeString ) || (psActClassName && (*psActClassName==aTypeString)) )
	{
		return Object;
	}

	// neu seit 14.2.2003
	if( IsTemplateType( aTypeString ) )
	{
		return TemplateType;
	}

	if( aTypeString==_VOID )
		return Void;
	else if( aTypeString==_BOOL )
		return Bool;
	else if( aTypeString==_STRING )
		return String;
	else if( aTypeString==_CHAR || aTypeString==_UCHAR )
		return CharTT;
	else if( aTypeString==_ULONG || aTypeString==_USHORT || aTypeString==_UINT ||
			 aTypeString==_LONG || aTypeString==_SHORT || aTypeString==_INT )
		return Int;
	else if( aTypeString==_FLOAT || aTypeString==_DOUBLE )
		return Double;
	else if( aTypeString==_FUNCTION )
		return Function;
	else
		return Unknown;
}
