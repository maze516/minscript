/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: e:\\home\\cvsroot/minscript/minip.cpp,v $
 *
 *  $Revision: 1.2 $
 *
 *	$Log: minip.cpp,v $
 *	Revision 1.2  2004/01/04 15:44:15  min
 *	string_read(), splitpath() added, new, improved native functioninterface implemented
 *	
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

#include "platform.h"

#include "mstl_iostream.h"
#include "mstl_fstream.h"

#include "osdep.h"
#include "minip.h"
#include "minparsernodes.h"
#ifdef _with_preproc
#include "minpreproc.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined( _MSC_VER )
#define PUTENV _putenv
#else
#define PUTENV putenv
#endif

//*************************************************************************

void InitDefaultTokenizer( minTokenizer & m_aTokenizer )
{
	m_aTokenizer.AddToken( minToken( Whitespace, " " ) );
	m_aTokenizer.AddToken( minToken( Whitespace, "\t" ) );
	m_aTokenizer.AddToken( minToken( Whitespace, "\n\r" ) );
	m_aTokenizer.AddToken( minToken( Whitespace, "\r\n" ) );
	m_aTokenizer.AddToken( minToken( Whitespace, "\n" ) );
	m_aTokenizer.AddToken( minToken( Whitespace, "\r" ) );

	m_aTokenizer.AddToken( minToken( SpecialChar, "{", BLOCK_OPEN_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, "}", BLOCK_CLOSE_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, "(", PARENTHIS_OPEN_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, ")", PARENTHIS_CLOSE_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, ";", STATEMENT_END_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, "]", ARRAY_STOP_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, "##", PREPROC_CONCATENATE_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, "#", PREPROC_START_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, ":", DOT_DOT_ID ) );
	m_aTokenizer.AddToken( minToken( SpecialChar, "\\", BACKSLASH_ID ) );

	m_aTokenizer.AddToken( minToken( Operator, "?", CONDITION_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, ".", DOT_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, ",", COMMA_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "[", ARRAY_START_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "!=", NOT_EQUAL_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "!", NOT_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "~", INVERT_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "++", INC_ID ) );		// WICHTIG: doppel-Token muss vor einfach-Token stehen !
	m_aTokenizer.AddToken( minToken( Operator, "+", ADD_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "->", PTR_DEREF_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "--", DEC_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "-", SUB_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "*", MULT_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "/", DIV_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "%", MOD_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "<=", LESSEQUAL_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "<<", LEFTSHIFT_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "<", LESS_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, ">=", MOREEQUAL_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, ">>", RIGHTSHIFT_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, ">", MORE_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "==", EQUAL_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "=", ASSIGN_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "&&", LOG_AND_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "||", LOG_OR_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "&", BINARY_AND_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "|", BINARY_OR_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "new", NEW_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "delete", DELETE_ID ) );
	m_aTokenizer.AddToken( minToken( Operator, "sizeof", SIZEOF_ID ) );

#ifdef USEBIG
	m_aTokenizer.AddToken( minToken( Operator, "__exists", EXISTS_ID ) );		// Erweiterung !
	m_aTokenizer.AddToken( minToken( Operator, "_*exists", DEREFEXISTS_ID ) );	// Erweiterung !
	m_aTokenizer.AddToken( minToken( Operator, "__dbghalt", DBGHALT_ID ) );		// Erweiterung !
	m_aTokenizer.AddToken( minToken( Operator, "typeof", TYPEOF_ID ) );			// Erweiterung !
#endif

	m_aTokenizer.AddToken( minToken( Keyword, _VOID, VOID_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _BOOL, BOOL_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _CHAR, CHAR_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _INT, INT_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _SHORT, SHORT_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _LONG, LONG_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _UNSIGNED, UNSIGNED_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _FLOAT, FLOAT_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _DOUBLE, DOUBLE_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, _STRING, STRING_ID ) );

	m_aTokenizer.AddToken( minToken( Keyword, "class", CLASS_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "struct", STRUCT_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "const", CONST_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "virtual", VIRTUAL_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "private", PRIVATE_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "protected", PROTECTED_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "public", PUBLIC_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "if", IF_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "else", ELSE_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "for", FOR_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "while", WHILE_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "do", DO_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "return", RETURN_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "break", BREAK_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "continue", CONTINUE_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "typedef", TYPEDEF_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "this", THIS_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "extern", EXTERN_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "static", STATIC_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "switch", SWITCH_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "case", CASE_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "default", DEFAULT_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "goto", GOTO_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "operator", OPERATOR_ID ) );
	m_aTokenizer.AddToken( minToken( Keyword, "template", TEMPLATE_ID ) );

	// zum Testen
#ifdef _with_object_keyword
	m_aTokenizer.AddToken( minToken( Keyword, "object", OBJECT_ID ) );
#endif
}

//*************************************************************************

// TODO: zur Zeit wird eine globale Variable verwendet,
// besser die Interpreter-Instanz an die native Funktion weiterreichen !!!
minInterpreterOutputInterface *	g_pOutputInterface = 0;

//*************************************************************************

// wird verwendet zum Laden und Entladen von DLLS
minScriptInterpreter *	g_pActualGlobalInterpreter = 0;


minScriptInterpreter::minScriptInterpreter( bool bDebug )
    : m_aEnvironment(),
      m_aTokenizer(),
      m_aParser( &m_aTokenizer ),	  
      m_bRunOk( false ),
      m_bDebug( bDebug ),
	  m_pOutputInterface( 0 )
{
	InitTokenizer();
	//InitRuntimeEnvironment();		wird in Run() ausgefuehrt

	g_pActualGlobalInterpreter = this;
}

minScriptInterpreter::~minScriptInterpreter()
{
	g_pActualGlobalInterpreter = 0;
}

void minScriptInterpreter::SetDebugModus( bool bDebug )
{
    m_bDebug = bDebug;
}

bool minScriptInterpreter::GetDebugModus() const
{
    return m_bDebug;
}

void minScriptInterpreter::SetDbgModus( bool bDebug )
{
    m_bDbg = bDebug;
}

bool minScriptInterpreter::GetDbgModus() const
{
    return m_bDbg;
}

void minScriptInterpreter::DumpAllFunctionPrototypes( ostream & aStream ) const
{
	m_aEnvironment.DumpAllFunctionPrototypes( aStream );
}

bool minScriptInterpreter::Run( const string & sScriptWithPredefs, const string & sScript, int nLineCountOfAddedCode, minInterpreterValue & aReturnValueOut, unsigned long * pExecutionTime, unsigned long * pParseTime, const minTokenizer::TokenContainerT & aParsedTokens )
{
	if( aParsedTokens.size()>0 )
	{
		m_aTokenizer.SetParsedTokens( &aParsedTokens );
		//m_aTokenizer.DumpTokenContainer( cout, aParsedTokens );
	}
	else
	{
		m_aTokenizer.SetText( sScript, nLineCountOfAddedCode );
	}
	unsigned long nParseStartTime = minGetCurrentTickCount();
	if( m_aParser.Parse( nLineCountOfAddedCode ) )
	{
		unsigned long nParseStopTime = minGetCurrentTickCount();
		minInterpreterNode * pNode = m_aParser.GetProgramNode();
		if( pNode )
		{
			do
			{
				// Environment initalisieren
				m_aEnvironment.RemoveAllFunctions();
				InitRuntimeEnvironment();
				// Callstack-Eintrag erzeugen, ausfuehren und Callstack-Eintrag wieder loeschen
				m_aEnvironment.SetLineCountOfAddedCode( nLineCountOfAddedCode );
				m_aEnvironment.PushCallStackItem( "__main()" );
				m_aEnvironment.ResetDebuggerExecutionFlags();
				m_aEnvironment.SetDbgResetExecution(false);
				m_aEnvironment.SetDebugMode( m_bDebug );
				m_aEnvironment.SetSourceCode( sScriptWithPredefs );
				m_aEnvironment.SetDbgMode( m_bDbg );
				if (m_aEnvironment.IsDbgMode())
				{
					cout << endl << "starting debugging..." << endl << endl;
				}
				unsigned long nStartTime = minGetCurrentTickCount();
				try 
				{
					m_bRunOk = pNode->Execute( /*bGetLValue*/false, aReturnValueOut, m_aEnvironment);
				}
				catch( minRuntimeException aError )
				{
					cerr << endl << "Runtime exception: " << aError.GetInfoString().c_str() << endl << endl;
				}
				unsigned long nStopTime = minGetCurrentTickCount();
				if (m_aEnvironment.IsDbgMode())
				{
					cout << endl << "script execution finished." << endl;
				}
				m_aEnvironment.PopCallStackItem();
				// Environment wieder zuruecksetzen
				//m_aEnvironment.RemoveAllFunctions();
				if( pExecutionTime )
				{
					*pExecutionTime = (1000*(nStopTime-nStartTime))/minGetTickCountPerSec();
				}
				if( pParseTime )
				{
					*pParseTime = (1000*(nParseStopTime-nParseStartTime))/minGetTickCountPerSec();
				}
			} while( m_aEnvironment.IsDbgMode() );

			return m_bRunOk;
		}
	}
	else
	{
		// TODO --> Parser-Fehler behandeln !
		ShowErrorPosition();
	}

	return false;
}

bool minScriptInterpreter::ParseOnly( const string & sScript, int nLineCountOfAddedCode )
{
	m_aTokenizer.SetText( sScript, nLineCountOfAddedCode );
	if( m_aParser.Parse( nLineCountOfAddedCode ) )
	{
		return true;
	}
	return false;
}

#ifdef USEBIG
bool minScriptInterpreter::GenerateCppCode( const string & sScript, string & sCppCodeOut )
{
	minInterpreterNode * pProgramNode = GetProgramNodeForScript( sScript );
	if( pProgramNode )
		return pProgramNode->GenerateCppCode( sCppCodeOut );

	ShowErrorPosition();
	return false;
}
#endif

bool minScriptInterpreter::RunPreProcessor( bool bOnlyPreproc, const string & sScriptIn, int nLineCountOfAddedCode, string & sPreProcedScriptOut, const StringListT & aIncludeDirList, minTokenizer::TokenContainerT & aParsedTokens )
{
    //cout << "PREPROCESSOR: " << sScriptIn << endl;
#ifdef _with_preproc
	minPreProcessor aPreProc( bOnlyPreproc, m_aTokenizer, sScriptIn, nLineCountOfAddedCode, aIncludeDirList, aParsedTokens );

	bool bOk = aPreProc.GenerateOutput( sPreProcedScriptOut );

	return bOk;
#else
    return false;
#endif
}

bool minScriptInterpreter::LoadModule( const string & sDllNameIn, long * phDll )
{
	typedef bool (*minRegisterNativeFunctionsT)( minInterpreterEnvironmentInterface * aEnvironment );
	
	// DLL laden und Funktion minRegisterNativeFunctions ausfuehren, falls vorhanden

	long hDLL = minLoadLibrary( sDllNameIn.c_str() );
	if( hDLL )
	{
		if( phDll )
		{
			*phDll = hDLL;
		}

		minRegisterNativeFunctionsT pInitFcn = (minRegisterNativeFunctionsT)minGetProcAddress( hDLL, "minRegisterNativeFunctions" );
		if( pInitFcn )
		{
			(*pInitFcn)( &m_aEnvironment );
			return true;
		}
		else
		{
			cerr << "Error getting init-function from dll: " << sDllNameIn.c_str() << endl;
		}
	}
	else
	{
		cerr << "Error opening dll: " << sDllNameIn.c_str() << endl;
	}
	return false;
}

bool minScriptInterpreter::UnLoadModule( long hDll )
{
	if( hDll )
	{
		return minFreeLibrary( hDll );
	}
	return false;
}

bool minScriptInterpreter::GetAllFunctionPrototypes( minParserItemList & aItemListOut )
{
	return GetAllNodes( _FUNCTIONDECLARATIONNODE, aItemListOut );
}

bool minScriptInterpreter::GetAllClassPrototypes( minParserItemList & aItemListOut )
{
	return GetAllNodes( _CLASSNODE, aItemListOut );
}

bool minScriptInterpreter::GetAllNodes( const string & sNodeName, minParserItemList & aItemListOut )
{
	aItemListOut.erase( aItemListOut.begin(), aItemListOut.end() );

	minInterpreterNode * const pNode = m_aParser.GetProgramNode();
	if( pNode )
	{
		const string & sNodeClassName = pNode->GetClassName();

		// eigene RTTI durchfuehren und Klasse des pNode bestimmen
		if( sNodeClassName==_BLOCKNODE )
		{
			const minParserItemList & aBlockNodeList = ((minBlockNode *)pNode)->GetAllNodes();

			minParserItemList::const_iterator aIter = aBlockNodeList.begin();
			while( aIter != aBlockNodeList.end() )
			{
				const string & sNodeClassNameTemp = (*aIter)->GetClassName();
				if( sNodeClassNameTemp==sNodeName )
				{
					aItemListOut.push_back( *aIter );
				}
				++aIter;
			}
			return true;
		}
	}
	return false;
}

#ifdef USEBIG
bool minScriptInterpreter::DumpParser( ostream & aStream )
{
	minInterpreterNode * const pNode = m_aParser.GetProgramNode();
	if( pNode )
		return pNode->Dump( aStream );
	return false;
}
#endif

minInterpreterNode * const minScriptInterpreter::GetProgramNodeForScript( const string & sScript )
{
	m_aTokenizer.SetText( sScript, 0 );
	if( m_aParser.Parse( 0 ) )
	{
		minInterpreterNode * pNode = m_aParser.GetProgramNode();
		if( pNode )
		{
			return pNode;
		}
	}
	return 0;
}

int minScriptInterpreter::GetErrorCode() const
{
	if( !m_bRunOk )
		return RUNTIME_ERROR;
	if( m_aTokenizer.IsError() )
		return m_aTokenizer.GetErrorCode();
	if( m_aParser.IsError() )
		return m_aParser.GetErrorCode();
	return 0;
}

void minScriptInterpreter::SetInterpreterOutputInterface( minInterpreterOutputInterface * pOutputInterface )
{
	m_pOutputInterface = pOutputInterface;
	// TODO: unschoener Zugriff auf globale Variable !!!
	g_pOutputInterface = pOutputInterface;
}

void minScriptInterpreter::ShowErrorPosition()
{
	if( m_aParser.IsError() )
	{
		cout << endl << "ERROR in Parser: code = " << m_aParser.GetErrorCode() << endl;
	}
	if( /*m_aTokenizer.IsError()*/true )
	{
		cout << endl << "ERROR:" << endl;
		cout << "Error Code: " << m_aTokenizer.GetErrorCode() << endl;
		cout << "Error Pos.: " << m_aTokenizer.GetErrorPos() << endl;
		//cout << "Script: " << m_aTokenizer.GetText() << endl;
		cout << "Script until Error:  " << string( m_aTokenizer.GetText(), 0, m_aTokenizer.GetErrorPos() ) << endl;
		cout << endl;
	}
	//else
	//{
	//	cout << "NO ERROR !" << endl;
	//}
}

//*************************************************************************

/*
void MyVoidGulp( int & i )
{
	i = 999;
}

int MyGulp( void )
{
	return 99;
}

int MyGulp4( int & n )
{
	return 99*n;
}

int MyGulp3( int n )
{
	return 99;
}

int MyGulp2( int n )
{
	return 99*n;
}

int PrintLnFcn2( const char * sText, int i )
{
	cout << sText << endl;
	return 0;
}
*/

int LoadInterpreterModule( const char * sDllName )
{
	if( g_pActualGlobalInterpreter )
	{
		long hDll = 0;

		if( g_pActualGlobalInterpreter->LoadModule( sDllName, &hDll ) )
		{
			return (int)hDll;
		}
	}
	return 0;
}

int UnLoadInterpreterModule( int hDll )
{
	if( g_pActualGlobalInterpreter )
	{
		long hTempDll = (long)hDll;

		if( g_pActualGlobalInterpreter->UnLoadModule( hTempDll ) )
		{
			return 1;
		}
	}
	return 0;
}

int PrintLnFcn( const char * sText )
{
	cout << sText << endl;
	if( g_pOutputInterface )
	{
		g_pOutputInterface->Print( sText );
		g_pOutputInterface->PrintLn();
	}
	return 0;
}

int PrintFcn( const char * sText )
{
	cout << sText;
	if( g_pOutputInterface )
	{
		g_pOutputInterface->Print( sText );
	}
	return 0;
}

int ErrorPrintLnFcn( const char * sText )
{
	cerr << sText << endl;
	return 0;
}

string string_read()
{
	const int MAX = 1024;
	char sBuffer[MAX];

	cin.getline(sBuffer,MAX);

	return sBuffer;
}

int string_npos()
{
	return (int)string::npos;
}

int string_find( const char * sString, const char * sSearchText )
{
	string s( sString );

	return (int)s.find( sSearchText );
}

string string_substr( const char * s, int iStartPos, int iLength )
{
	return string( s ).substr( iStartPos, iLength );
}

int string_length( const char * s )
{
    return (int)strlen( s );
}

char string_at( const char * s, int iPos )
{
    if( iPos>=0 && iPos<(int)strlen( s ) )
        return s[iPos];
    return 0;
}

string string_setchar( const char * s, int iPos, char ch )
{
    string sTemp( s );
    if( iPos>=0 && iPos<(int)sTemp.length() )
        sTemp[iPos] = ch;
    else
        sTemp += ch;
    return sTemp;
}

string string_insert( const char * s, int iPos, const char * sInsert )
{
    return string( s ).insert( iPos, sInsert );
}

string string_erase( const char * s, int iPos, int iLength )
{
    return string( s ).erase( iPos, iLength );
}

string string_replace( const char * s, int iPos, int iLength, const char * sReplace )
{
    return string( s ).replace( iPos, iLength, sReplace );
}

int my_fgets( string & s, long hFile )
{
	FILE * pFile = (FILE *)hFile;

	const int c_iMAX = 512;
	char sBuffer[c_iMAX];

	if( fgets( sBuffer, c_iMAX, pFile ) )
	{
		s = sBuffer;
		return (int)s.length();
	}

	return 0;
}

string my_getenv( const char * s )
{
	string sRet;

	char * sValue = getenv( s );

	if( sValue )
	{
		sRet = s;
	}

	return sRet;
}

#if defined( __linux__ ) || defined( __APPLE__ )

static char _GetDirectorySeparator()
{
#if defined( __linux__ ) || defined( __APPLE__ )
	return '/';
#else
	return '\\';
#endif
}

// Hilfsfunktion fuer SplitPath()
static void _InsertChar( char * sBuffer, char ch )
{
	size_t nLen = strlen( sBuffer );
	sBuffer[ nLen ] = ch;
	sBuffer[ nLen+1 ] = 0;
}

// Hilfsfunktion fuer SplitPath()
// Transformation durchfuehren "abc" --> "cba"
static void _RotateString( char * sBuffer )
{
	char sBuf[512];
	size_t nLen = strlen( sBuffer );
    for( size_t i=0; i<nLen; i++ )
	{
		sBuf[ nLen-i-1 ] = sBuffer[i];
	}
	sBuf[ nLen ] = 0;
	strcpy( sBuffer, sBuf );
}

#endif

bool SplitPath( const char * sPath, string & sDrive, string & sDir, string & sFileName, string & sExt )
{
	char sDriveBuf[512];
	char sDirBuf[512];
	char sNameBuf[512];
	char sExtBuf[512];
#if !(defined( __linux__ ) || defined( __APPLE__ ))
	_splitpath( /*(CHAR_CAST)*/sPath, sDriveBuf, sDirBuf, sNameBuf, sExtBuf );
#else
	// Simuliere _splitpath fuer Linux...
// TODO --> dirname() basename()    
	strcpy( sDriveBuf, "" );
	strcpy( sDirBuf, "" );
	strcpy( sNameBuf, "" );
	strcpy( sExtBuf, "" );
	// Pfad von hinten analysieren
	char sBuffer[512];
	strcpy( sBuffer, "" );
	bool bFoundExt = false;
	bool bFoundFile = false;
	for( int i=(int)strlen( sPath )-1; i>=0; i-- )
	{
		if( !bFoundExt && sPath[i]=='.' )
		{
			bFoundExt = true;
			_InsertChar( sBuffer, sPath[i] );
			_RotateString( sBuffer );
			strcpy( sExtBuf, sBuffer );
			strcpy( sBuffer, "" );
		}
		else if( !bFoundFile && sPath[i]==/*FileUtilityObj::*/_GetDirectorySeparator() )
		{
			bFoundFile = true;
			_RotateString( sBuffer );
			strcpy( sNameBuf, sBuffer );
			strcpy( sBuffer, "" );
			_InsertChar( sBuffer, sPath[i] );
		}
		else
		{
			_InsertChar( sBuffer, sPath[i] );
		}
	}
	_RotateString( sBuffer );
	if( bFoundFile )
	{
		strcpy( sDirBuf, sBuffer );
	}
	else
	{
		strcpy( sNameBuf, sBuffer );
	}
#endif
	sDrive = sDriveBuf;
	sDir = sDirBuf;
	sFileName = sNameBuf;
	sExt = sExtBuf;
	//cout << "SPLIT: " << sDirBuf << " *** " << sNameBuf << " *** " << sExtBuf << endl;
	return true;
}

/*
int my_ref_test( int & iInOut )
{
	cout << "my_ref_test()  in=" << iInOut << endl;
	iInOut = 2*iInOut;
	cout << "my_ref_test() out=" << iInOut << endl;
	return iInOut;
}

string my_ref_test_string( string & sInOut )
{
	cout << "my_ref_test_string()  in=" << sInOut.c_str() << endl;
	sInOut = sInOut + sInOut;
	cout << "my_ref_test_string() out=" << sInOut.c_str() << endl;
	return sInOut;
}
*/

//    vector<minInterpreterValue> aVector;
//    aVector.push_back( minInterpreterValue( 3 ) );

//*************************************************************************
// Hilfsklasse
/*
class _ResolveTypeToCompileTime
{
public:
	_ResolveTypeToCompileTime() : m_nVal( Unknown ), m_bIsReference( false ) {}
	_ResolveTypeToCompileTime( bool nVal ) : m_nVal( Bool ), m_bIsReference( false ) {}
	_ResolveTypeToCompileTime( char nVal ) : m_nVal( Char ), m_bIsReference( false ) {}
	_ResolveTypeToCompileTime( int nVal ) : m_nVal( Int ), m_bIsReference( false ) {}
	_ResolveTypeToCompileTime( int & nVal ) : m_nVal( Int ), m_bIsReference( true ) {}
	_ResolveTypeToCompileTime( double nVal ) : m_nVal( Double ), m_bIsReference( false ) {}
	_ResolveTypeToCompileTime( const string & nVal ) : m_nVal( String ), m_bIsReference( false ) {}

	InterpreterValueType	GetType() const			{ return m_nVal; };
	bool					GetIsReference() const	{ return m_bIsReference; }

private:
	InterpreterValueType	m_nVal;
	bool					m_bIsReference;
};
*/
/*
	Implementations-Bemerkungen zum Native-Function-Interface des Interpreters:

	Wrapper fuer Interpreter:
	Fuer jede Native-Funktion wird eine 'Wrapper-Funktion' fuer den Interpreter
	benoetigt, da der Interpreter nur seine eigenen Variablen-Typen (int, double, etc.)
	kennt und dementsprechend eine Konversion in die passenden C++-Typen vorgenommen
	werden muss.
	Diese Wrapper-Funktion wird 'automatisch' zur Compile-Zeit (d.h. mit Hilfe von
	Templates) erzeugt.
	Die Wrapper-Funktion wird durch eine Klasse/Objekt repraesentiert. Dieses verwaltet
	den Namen der Funktion, den Return-Typ und die Parameter-Namen und -Typen.
	Es fuehrt die Konversion der Parameter und den Aufruf der Funktion durch:

		call-operator( p1, p2, ... )
		{
			// konvertiere alle Parameter vom Interpreter-Format in C++-Typ
			// rufe die native-Funktion mit den notwendigen Parametern auf
			// konvertiere alle notwendigen Parameter (call-by-reference) von den C++-Typen in das Interpreter-Format
			// konvertiere und returne den Return-Wert
		}

	Der Interpreter kann dann zur Laufzeit, falls notwendig fuer einen Funktionsaufruf,
	das Wrapper-Objekt fuer die native-Funktion verwenden um den Aufruf durchzufuehren !
	Siehe auch Klasse: minNativeFunctionDeclarationNode
*/

//#########################################################################
//#########################################################################
//#########################################################################
/*
bool minScriptInterpreter::RegisterNativeFcn( minNativeFcnWrapperBaseAdapter * pNewNativeFcn )
{
	return m_aEnvironment.AddFunction( minHandle<minFunctionDeclarationNode>( new minNativeFunctionDeclarationNode( pNewNativeFcn ) ) );
}
*/
/* Verfuegbare Funktionen:

	double fabs( double d )
	double sin( double d )
	double asin( double d )
	double sinh( double d )
	double cos( double d )
	double acos( double d )
	double cosh( double d )
	double tan( double d )
	double atan( double d )
	double atan2( double d1, double d2 )
	double tanh( double d )
	double sqrt( double d )
	double exp( double d )
	double log( double d )
	double log10( double d )
	double pow( double d1, double d2 )

	int fopen( string sFileName, string sMode );
	int fclose( int hFile );
	int feof( int hFile );
	int ferror( int hFile );
	int fputs( string sText, int hFile );
	int fgets( string & sText, int hFile );

	int string_read()
	int string_npos()
	int string_length( string s )
	char string_at( string s, int iPos )
	string string_setchar( string s, int iPos, char ch )
	int string_find( string s, string sSearch )
	string string_substr( string s, int iStartPos, int iLength )
	string string_insert( string s, int iPos, string sInsert )
	string string_erase( string s, int iPos, int iLength )
	string string_replace( string s, int iPos, int iLength, string sReplace )

    int current_time_hour()
    int current_time_minute()
    int current_time_second()
    int current_date_year()
    int current_date_month()
    int current_date_day()

	string getenv( string sName )
	int putenv( string sNameValue )
	void exit( int iValue ) int system( string sCmd )
	int system( string sCmd )

	int __println( string s )
	int __errorln( string s )
	void __sleep( int iDelay )
	int __loaddll( string s )
	int __unloaddll( int hDll )
    long clockms()
*/

long _GetCurrentTickCount()
{
	return (long)GetCurrentTickCount();
}

int CurrentTimeHour()
{
    time_t aTime;
    time(&aTime);
    
    struct tm * tm_struct = localtime(&aTime);

    return tm_struct->tm_hour;
}

int CurrentTimeMinute()
{
    time_t aTime;
    time(&aTime);
    
    struct tm * tm_struct = localtime(&aTime);

    return tm_struct->tm_min;
}

int CurrentTimeSecond()
{
    time_t aTime;
    time(&aTime);
    
    struct tm * tm_struct = localtime(&aTime);

    return tm_struct->tm_sec;
}

int CurrentDateYear()
{
    time_t aTime;
    time(&aTime);
    
    struct tm * tm_struct = localtime(&aTime);

    return tm_struct->tm_year;
}

int CurrentDateMonth()
{
    time_t aTime;
    time(&aTime);
    
    struct tm * tm_struct = localtime(&aTime);

    return tm_struct->tm_mon;
}

int CurrentDateDay()
{
    time_t aTime;
    time(&aTime);
    
    struct tm * tm_struct = localtime(&aTime);

    return tm_struct->tm_mday;
}

void minScriptInterpreter::InitRuntimeEnvironment()
{
	NativeFcnWrapperBase * pFcn = 0;

	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)fabs, "double fabs( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)sin, "double sin( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)asin, "double asin( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)sinh, "double sinh( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)cos, "double cos( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)acos, "double acos( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)cosh, "double cosh( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)tan, "double tan( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)atan, "double atan( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper2<double,double,double>( (NativeFcnWrapper2<double,double,double>::MyFcnType2)atan2, "double atan2( double d1, double d2 );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)tanh, "double tanh( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)sqrt, "double sqrt( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)exp, "double exp( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)log, "double log( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<double,double>( (NativeFcnWrapper1<double,double>::MyFcnType1)log10, "double log10( double d );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper2<double,double,double>( (NativeFcnWrapper2<double,double,double>::MyFcnType2)pow, "double pow( double d1, double d2 );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	// string.h functions
	pFcn = new NativeFcnWrapper0<string>( string_read, "string string_read();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper0<int>( string_npos, "int string_npos();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int, const char *>( string_length, "int string_length( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper2<char, const char *, int>( string_at, "char string_at( string s, int iPos );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper3<string, const char *, int, char>( string_setchar, "string string_setchar( string s, int iPos, char ch );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper2<int, const char *, const char *>( string_find, "int string_find( string s, string sSearch );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper3<string, const char *, int, int>( string_substr, "string string_substr( string s, int iStartPos, int iLength );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper3<string, const char *, int, const char *>( string_insert, "string string_insert( string s, int iPos, string sInsert );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper3<string, const char *, int, int>( string_erase, "string string_erase( string s, int iPos, int iLength );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper4<string, const char *, int, int, const char *>( string_replace, "string string_replace( string s, int iPos, int iLength, string sReplace );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	// file-io functions
	pFcn = new NativeFcnWrapper2<int, const char *, const char *>( (NativeFcnWrapper2<int, const char *, const char *>::MyFcnType2)fopen, "int fopen( string sFileName, string sMode );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int, int>( (NativeFcnWrapper1<int, int>::MyFcnType1)fclose, "int fclose( int hFile );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int, int>( (NativeFcnWrapper1<int, int>::MyFcnType1)feof, "int feof( int hFile );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int, int>( (NativeFcnWrapper1<int, int>::MyFcnType1)ferror, "int ferror( int hFile );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper2<int, const char *, int>( (NativeFcnWrapper2<int, const char *, int>::MyFcnType2)fputs, "int fputs( string sFileName, int hFile );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new RefNativeFcnWrapper2<int, _Ref<string>, _Val<int> >( (RefNativeFcnWrapper2<int, _Ref<string>, _Val<int> >::MyFcnType2)my_fgets, "int fgets( string & sText, long hFile );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	// stdlib.h
	pFcn = new NativeFcnWrapper1<string, const char *>( (NativeFcnWrapper1<string, const char *>::MyFcnType1)my_getenv, "string getenv( string sName );" );
	m_aEnvironment.AddNativeFunction( pFcn );
    pFcn = new NativeFcnWrapper1<int, const char *>( (NativeFcnWrapper1<int, const char *>::MyFcnType1)PUTENV, "int putenv( string sNameValue );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeVoidFcnWrapper1<int>( (NativeVoidFcnWrapper1<int>::MyFcnType1)exit, "void exit( int iValue );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	pFcn = new NativeFcnWrapper1<int,const char *>( MySystem, "int system( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	pFcn = new RefNativeFcnWrapper5<bool,_Val<const char *>,_Ref<string>,_Ref<string>,_Ref<string>,_Ref<string> >( (RefNativeFcnWrapper5<bool,_Val<const char *>,_Ref<string>,_Ref<string>,_Ref<string>,_Ref<string> >::MyFcnType5)SplitPath, "bool splitpath( string sPath, string & sDrive, string & sDir, string & sFileName, string & sExt );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	// buildin functions
	pFcn = new NativeFcnWrapper1<int,const char *>( PrintLnFcn, "int PrintLn( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int,const char *>( PrintLnFcn, "int println( string s );" );
    m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int,const char *>( PrintFcn, "int print( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int,const char *>( PrintFcn, "int __print( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int,const char *>( PrintLnFcn, "int __println( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int,const char *>( ErrorPrintLnFcn, "int __errorln( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	//pFcn = new NativeFcnWrapper2<int, const char *, int>( PrintLnFcn2, "void PrintLn( string s, int i );" );
	//m_aEnvironment.AddNativeFunction( pFcn );

	pFcn = new NativeFcnWrapper0<long>( _GetCurrentTickCount, "long clockms();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int,int>( WaitFcn, "void __sleep( int iDelay );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	// laden von Modulen
	pFcn = new NativeFcnWrapper1<int,const char *>( LoadInterpreterModule, "int __loaddll( string s );" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper1<int,int>( UnLoadInterpreterModule, "int __unloaddll( int hDll );" );
	m_aEnvironment.AddNativeFunction( pFcn );

	pFcn = new NativeFcnWrapper0<int>( CurrentTimeHour, "int current_time_hour();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper0<int>( CurrentTimeMinute, "int current_time_minute();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper0<int>( CurrentTimeSecond, "int current_time_second();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper0<int>( CurrentDateYear, "int current_date_year();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper0<int>( CurrentDateMonth, "int current_date_month();" );
	m_aEnvironment.AddNativeFunction( pFcn );
	pFcn = new NativeFcnWrapper0<int>( CurrentDateDay, "int current_date_day();" );
	m_aEnvironment.AddNativeFunction( pFcn );

	// test
	//pFcn = new RefNativeFcnWrapper1<int, _Ref<int> >( (RefNativeFcnWrapper1<int, _Ref<int> >::MyFcnType1)my_ref_test, "int my_ref_test( int & iInOut );" );
	//m_aEnvironment.AddNativeFunction( pFcn );
	//pFcn = new RefNativeFcnWrapper1<string, _Ref<string> >( (RefNativeFcnWrapper1<string, _Ref<string> >::MyFcnType1)my_ref_test_string, "string my_ref_test_string( string & sInOut );" );
	//m_aEnvironment.AddNativeFunction( pFcn );

    // container operations (for stl-container)
	//pFcn = new NativeFcnWrapper1<int, minInterpreterValue>( container_push_back, "int container_push_back( object aObj );" );
	//m_aEnvironment.AddNativeFunction( pFcn );

	// Beispiel fuer void-Funktion
	//pFcn = new NativeVoidFcnWrapper1<int &>( MyVoidGulp, "void voidgulp( int & i );" );
	//m_aEnvironment.AddNativeFunction( pFcn );

	// hier Problem bei C++-Code-Erzeugung, da Funktionsprototypen fehlen und Name ggf. unterschiedlich sein kann !!!
	//pFcn = new NativeFcnWrapper1<int,int>( MyGulp2, "int mygulp( int i );" );
	//m_aEnvironment.AddNativeFunction( pFcn );
	//pFcn = new NativeFcnWrapper1<int,int>( WaitFcn, "void Wait( int iDelay );" );
	//m_aEnvironment.AddNativeFunction( pFcn );
}

void minScriptInterpreter::InitTokenizer()
{
	InitDefaultTokenizer( m_aTokenizer );

#ifndef _slow_search_
	m_aTokenizer.SortTokenContainer();
#endif
}

//*************************************************************************
//*************************************************************************
//*************************************************************************

static string CheckForFinalDirSeparator( const string & sDirectory )
{
	string sRet( sDirectory );

	if( sRet.length()>0 )
	{
		if( sRet[ sRet.length() - 1 ] != _DIRECTORY_SEPARATOR )
		{
			sRet += _DIRECTORY_SEPARATOR;
		}
	}

	return sRet;
}

static bool ExistsFile( const char * sFileName )
{
	ifstream aTestFile( sFileName, /*org: ios::nocreate*/ios::in );

	return aTestFile.good();
}

bool ReadScript( const char * sFileName, string & sScriptOut, const StringListT & aIncludeDirList )
{
	string	sFileNameTemp( sFileName );
	bool	bExists;
	StringListT::const_iterator aIter = aIncludeDirList.begin();

	do
	{
		bExists = ExistsFile( sFileNameTemp.c_str() );

		if( bExists )
		{
			fstream aFile( sFileNameTemp.c_str(), ios::in );

			if( aFile.good() )
			{
				sScriptOut = "";

				while( !aFile.eof() )
				{
					char sBuffer[1024];

					strcpy( sBuffer, "" );
					aFile.getline( sBuffer, 1023 );
                    //if( strlen(sBuffer)>0 )	// 16.7.2015: do not ignore empty lines
                    {
                        sScriptOut += sBuffer;
                        sScriptOut += "\n";     // TODO: dieses neue Zeile Zeichen ist manchmal falsch, wenn letzte Zeile im Skript kein new line hat !
                    }
				}
				return true;
			}
		}	
		else
		{
			if( aIter != aIncludeDirList.end() )
			{
				sFileNameTemp = CheckForFinalDirSeparator( *aIter ) + sFileName;

				++aIter;
			}
			else
			{
				return false;
			}
		}
	}
	while( !bExists );

    return false;
}

bool WriteAsciiFile( const char * sFileName, const string & sTextIn )
{
	fstream aFile( sFileName, ios::out );

	if( aFile.good() )
	{
		aFile << sTextIn.c_str();
		return true;
	}
	return false;
}

// http://stackoverflow.com/questions/236129/split-a-string-in-c
vector<string> split(const string & str, const string & delimiters)
{
	vector<string> v;
	string::size_type start = 0;
	auto pos = str.find_first_of(delimiters, start);
	while (pos != string::npos)
	{
		//if (pos != start) // ignore empty tokens
		{
			//			v.emplace_back(str, start, pos - start);
			v.push_back(str.substr(start, pos - start));
		}
		start = pos + 1;
		pos = str.find_first_of(delimiters, start);
	}
	if (start < str.length()) // ignore trailing delimiter
	{
		//		v.emplace_back(str, start, str.length() - start); // add what's left of the string
		v.push_back(str.substr(start, str.length() - start));
	}
	return v;
}

void DumpScript( const string & sScript, int nLineCodeOfAddedCode, int nCurrentLineNo, list<int> lstBreakpointLines )
{
	vector<string> lines = split(sScript, string("\n"));
	int iLineNo = 1 - nLineCodeOfAddedCode;
	vector<string>::const_iterator iter = lines.begin();
	while( iter != lines.end() )
	{		
		if (find(lstBreakpointLines.begin(), lstBreakpointLines.end(), iLineNo) != lstBreakpointLines.end())
		{
			cout << "B";
		}
		else
		{
			cout << " ";
		}
		if( iLineNo == nCurrentLineNo )
		{
			cout << "-> ";
		}
		else
		{
			cout << "   ";
		}
		cout << iLineNo << " " << *iter << endl;
		iLineNo++;
		iter++;
	}
}

int CountNewLines( const string & s )
{
	size_t n = std::count(s.begin(), s.end(), '\n');
	return n;
}

#ifdef _with_preproc
//template list<string>;
#endif

#ifdef _with_digit_container
//template list<char>;
#endif

