/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/mintokenizer.h,v $
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

#ifndef _MINTOKENIZER_H
#define _MINTOKENIZER_H

//*************************************************************************
#include "mstl_string.h"
#include "mstl_list.h"
#include "mstl_vector.h"
#include "mstl_iostream.h"

//*************************************************************************
#include "dllexport.h"
#include "mintoken.h"

//*************************************************************************

// +++ Fehler-Id's des Tokenizers +++
const int ERROR_NO_ERROR =      0;
const int ERROR_UNKNOWN_TOKEN =	100;
const int ERROR_IN_COMMENT =	101;
const int ERROR_IN_STRING =		102;
const int ERROR_IN_CHARCONST =	103;

// +++ Fehler-Id's des Parsers +++
const int EXPECT_IDENTIFIER_OR_CONSTANT		= 1000;
const int EXPECT_PARENTHIS_CLOSE			= 1001;
const int ERROR_IN_PARENTHIS				= 1002;
const int UNEXPECTED_TOKEN					= 1003;
const int UNEXPECTED_KEYWORD				= 1004;
const int EXPECTED_SEPARATOR				= 1005;
const int ERROR_IN_RETURN					= 1006;
const int ERROR_IN_EXPRESSION				= 1007;
const int EXPECT_IDENTIFIER					= 1008;
const int ERROR_READING_TOKEN				= 1009;
const int CANT_RETURN_REFERENCE				= 1010;
const int REFERENCE_NEEDS_LVALUE			= 1011;
const int ARRAY_STOP_EXPECTED				= 1012;
const int ERROR_IN_CLASS					= 1013;
const int EXPECTED_CLASS_NAME				= 1014;
const int CLASS_NAME_ALREADY_DEFINED		= 1015;
const int ERROR_IN_ARRAY_INDEX				= 1016;
const int INITIALIZION_NOT_ALLOWED			= 1017;
const int UNKNOWN_VARIABLE					= 1018;
const int NEED_TYPE_FOR_NEW					= 1019;
const int ERROR_IN_FCN_CALL					= 1020;
const int ERROR_IN_DEFINED					= 1021;
const int EXPECTED_INHERITANCE_TOKEN		= 1022;
const int EXPECTED_BASECLASS				= 1023;
const int UNKNOWN_BASECLASS					= 1024;
const int ERROR_IN_BASECLASS_INIT			= 1025;
const int UNKNOWN_OPERATOR					= 1026;

const int TEST								= 1099;

//*************************************************************************

#define _TRUE	"true"
#define _FALSE	"false"

//*************************************************************************
// +++ einige wichtige Typen +++
typedef list<string>	StringListT;

//*************************************************************************
class minTokenizerState;

//*************************************************************************
/** Klasse wandelt String in Tokens */
class MINDLLEXPORT minTokenizer
{
	//minTokenizer( const minTokenizer & );
	minTokenizer & operator=( const minTokenizer & );

public:
	typedef vector<minToken>	TokenContainerT;		// bis 4.4.2001 list

	minTokenizer();

	void	SetParsedTokens( const TokenContainerT * pTokenContainer );

	// setzt den Programmtext
	void	SetText( const string & sProgramTextIn, int nLineCountOfAddedCode )	{ m_sProgram = sProgramTextIn; InitProcessing( nLineCountOfAddedCode ); }
	string  GetText() const								{ return m_sProgram; }

	void	SetReplaceStringEscapesModus( bool bDoReplace )		{ m_bDoReplace = bDoReplace; }
	bool	GetReplaceStringEscapesModus() const				{ return m_bDoReplace; }

	// initialisiert den Tokenizer-Prozess an den Anfang des Programmtextes
	bool InitProcessing( int nLineCountOfAddedCode );

	// Ueberpruefung auf Fehlerzustand und Fehlerposition
	bool IsError() const							{ return m_bIsError; }
	int  GetErrorCode() const						{ return m_nErrorCode; }
	int  GetErrorPos() const						{ return (int)(m_sProgram.size()-m_sParserString.size()); }

	// Abfrage des naechsten Tokens
	bool ReadNextToken();
	bool GetNextToken( minToken & aTokenOut );
	bool PeekNextToken( minToken & aTokenOut );
	// liefert das naechste Token, das kein Kommentar, Whitespace, etc. ist
	bool GetRealToken( minToken & aTokenOut, bool bIsNewLineWhitespace = false );
	bool PeekRealToken( minToken & aTokenOut, bool bIsNewLineWhitespace = false );

	// zum merken eines Tokenizer-Zustandes
	minTokenizerState * SaveState();
	bool RestoreState( minTokenizerState * pState );
	bool DeleteState( minTokenizerState * pState );

	// zum Initialisieren aller gueltigen Tokens )
	void AddToken( const minToken & aNewTokenIn );

	void SortTokenContainer();

	static string GetStringEscapeChar( char ch );

	// zum Debuggen
	void DumpTokenContainer( ostream & aStream, const TokenContainerT & aTokenContainer ) const;

private:
	// +++ Hilfsmethoden +++
	bool ReadChar();
	bool ReadChar( char & ch );
	bool ReadCharIfFound( char ch );
	bool ReadStringIfFound( const string & sString );
	char ReadDigitIfFound( bool bIsHex = false );
	char ReadIdentifierCharIfFound();
	bool ReplaceStringEscapeChars( string & sStringInOut ) const;
	bool IsIdentifierChar( char ch );
	bool CheckForIntToken( minToken & aTokenInOut );
	bool CheckForKnownToken( minToken & aTokenOut, bool bRead );
	bool CheckForKnownTokenReplaceInt( minToken & aTokenOut );
	bool CheckForNumberToken( minToken & aTokenOut );
	bool CheckForCharToken( minToken & aTokenOut );
	bool CheckForStringToken( minToken & aTokenOut );
	bool CheckForBoolToken( minToken & aTokenOut );
	bool CheckForCommentToken( minToken & aTokenOut );
	bool CheckForIdentifierToken( minToken & aTokenOut );

	// +++ Daten +++
	string				m_sProgram;						// State
	string				m_sParserString;				// State, Arbeitsvariable zum Parsen
	bool				m_bTokenAvailable;				// State
	minToken			m_aPeekedToken;					// State

	bool				m_bDoReplace;					// Flag
	bool				m_bReplayTokens;				// Flag

	bool				m_bIsError;						// fuer Fehlerbehandlung
	int					m_nErrorCode;					// fuer Fehlerbehandlung

	TokenContainerT		m_aTokenContainer;				// Arbeitscontainer

	const TokenContainerT *			m_pParsedTokenContainer;	// falls der Programmtext schon geparsed wurde 
	TokenContainerT::const_iterator	m_aParsedTokenIter;
	int								m_iCount;
    int                             m_iLineCount;

#ifdef _with_digit_container
	typedef list<char>	CharContainerT;

	CharContainerT		m_aIdentifierCharContainer;		// Arbeitscontainer
	CharContainerT		m_aDigitContainer;				// Arbeitscontainer

	void InitCharContainer();
#endif
};


#endif

