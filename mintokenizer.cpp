/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/mintokenizer.cpp,v $
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

#include "mintokenizer.h"

#include "mstl_algorithm.h"

#include <assert.h>

//#define _with_fast_token_search

#ifdef __linux__
// String-Klasse unter linux hat kein erase, sonder nur ein remove
//#define erase remove				// nur fuer alte Versionen von gcc notwendig
#endif

const char * g_sHexStart	= "0x";
const char * g_sEscapeChar	= "\\";
const char g_chEscapeChar	= g_sEscapeChar[0];

// Hilfsklasse zum merken des Tokenizer-Zustandes (28.12.1999)
class minTokenizerState
{
public:
	//string			m_sProgram;						// State
	string				m_sParserString;				// State, Arbeitsvariable zum Parsen
	bool				m_bTokenAvailable;				// State
	minToken			m_aPeekedToken;					// State

	minTokenizer::TokenContainerT::const_iterator	m_aParsedTokenIter;		// State
};


#ifdef _with_digit_container
void minTokenizer::InitCharContainer()
{
	m_aDigitContainer.push_back( '0' );
	m_aDigitContainer.push_back( '1' );
	m_aDigitContainer.push_back( '2' );
	m_aDigitContainer.push_back( '3' );
	m_aDigitContainer.push_back( '4' );
	m_aDigitContainer.push_back( '5' );
	m_aDigitContainer.push_back( '6' );
	m_aDigitContainer.push_back( '7' );
	m_aDigitContainer.push_back( '8' );
	m_aDigitContainer.push_back( '9' );

	char ch;
	for( ch='a'; ch<='z'; ch++ )
	{
		m_aIdentifierCharContainer.push_back( ch );
	}
	for( ch='A'; ch<='Z'; ch++ )
	{
		m_aIdentifierCharContainer.push_back( ch );
	}
	m_aIdentifierCharContainer.push_back( '_' );
}
#endif

minTokenizer::minTokenizer()
	: m_bDoReplace( true ), 
	  m_bReplayTokens( false ),
	  m_pParsedTokenContainer( 0 ),
      m_iCount(0),
      m_iLineCount(0)
{
#ifdef _with_digit_container
	InitCharContainer();
#endif
}

void minTokenizer::SetParsedTokens( const TokenContainerT * pTokenContainer )
{
	m_bReplayTokens = true;
	m_pParsedTokenContainer = pTokenContainer;
}

bool minTokenizer::InitProcessing( int nLineCountOfAddedCode )
{
	m_bIsError = false;
	m_nErrorCode = ERROR_NO_ERROR;
	m_sParserString = m_sProgram;
	m_bTokenAvailable = false;
	if( m_pParsedTokenContainer )
	{
		m_aParsedTokenIter = m_pParsedTokenContainer->begin();
	}
	m_iCount = 0;
    m_iLineCount = 1 - nLineCountOfAddedCode;		// start with line number 1 !
	return true;
}

bool minTokenizer::ReadNextToken()
{
	minToken aToken;
	return GetNextToken( aToken );
}

bool minTokenizer::GetNextToken( minToken & aTokenOut )
{
	if( !m_bTokenAvailable )
	{
		if( !PeekNextToken( aTokenOut ) )
			return false;
	}
	aTokenOut = m_aPeekedToken;
	m_bTokenAvailable = false;

	if( m_bReplayTokens )
	{
		++m_aParsedTokenIter;
		++m_iCount;
	}
	return true;
}

extern size_t CountNewLines(const string & s);

bool minTokenizer::PeekNextToken( minToken & aTokenOut )
{
	// falls es schon einen gepeekten Token gibt, diesen liefern
	if( m_bTokenAvailable )
	{
		aTokenOut = m_aPeekedToken;
		return true;
	}

	if( m_bReplayTokens && m_pParsedTokenContainer )
	{
		if( m_aParsedTokenIter != m_pParsedTokenContainer->end() )
		{
			m_aPeekedToken = *m_aParsedTokenIter;
			aTokenOut = m_aPeekedToken;
			m_bTokenAvailable = true;
			return true;
		}
		else
		{
			return false;
		}
	}

	minToken aNextToken;
	aNextToken.SetLineNo( m_iLineCount );

	// ist es ein bekanntes Token	
	// ACHTUNG: die Reihenfolge der Aufrufe ist WICHTIG (nicht aendern!)
	if( CheckForCommentToken( aNextToken ) ||
		CheckForKnownTokenReplaceInt( aNextToken ) ||	// Operatoren etc. testen --> TODO: Ausnahme -/+ vor Zahl !!
		CheckForNumberToken( aNextToken ) || 
		CheckForCharToken( aNextToken ) ||				// neu seit 24.12.1999
		CheckForStringToken( aNextToken ) ||
		CheckForBoolToken( aNextToken ) ||
		CheckForIdentifierToken( aNextToken ) )
	{
		// JA
		aTokenOut = aNextToken;
		m_aPeekedToken = aNextToken;
        if( aNextToken.IsNewLine() )
        {
            m_iLineCount++;
        }
		else if (aNextToken.IsComment())
		{
			m_iLineCount += (int)CountNewLines( aNextToken.GetString() );
		}
        else
        {
            //cout << "TOK: " << aNextToken.GetString() << endl;
        }
		m_bTokenAvailable = true;
		return true;
	}

	// falls der ParserString nicht ganz abgearbeitet wurde, 
	// so ist irgend etwas fehlerhaftes passiert.
	if( m_sParserString.size()>0 )
	{
		m_bIsError = true;
		m_nErrorCode = ERROR_UNKNOWN_TOKEN;
	}

	return false;
}

bool minTokenizer::GetRealToken( minToken & aTokenOut, bool bIsNewLineWhitespace )
{
	if( PeekRealToken( aTokenOut, bIsNewLineWhitespace ) )
	{
		ReadNextToken();
		return true;
	}
	return false;
}

bool minTokenizer::PeekRealToken( minToken & aTokenOut, bool bIsNewLineWhitespace )
{
	bool bOk = false;

	// Bugfix: 23.12.1999 IsComment() hinzugefuegt
	while( !IsError() && (bOk=PeekNextToken( aTokenOut )) && 
		   ( (aTokenOut.IsWhitespace() && (bIsNewLineWhitespace ? true : !aTokenOut.IsNewLine() )) || aTokenOut.IsComment() ) )
	{
		ReadNextToken();
	}
	return !IsError() && bOk;
}

minTokenizerState * minTokenizer::SaveState()
{
	minTokenizerState * pState = new minTokenizerState;
	//pState->m_sProgram = m_sProgram;
	pState->m_sParserString = m_sParserString;
	pState->m_bTokenAvailable = m_bTokenAvailable;
	pState->m_aPeekedToken = m_aPeekedToken;
	pState->m_aParsedTokenIter = m_aParsedTokenIter;
	return pState;
}

bool minTokenizer::RestoreState( minTokenizerState * pState )
{
	if( pState )
	{
		//m_sProgram = pState->m_sProgram;
		m_sParserString = pState->m_sParserString;
		m_bTokenAvailable = pState->m_bTokenAvailable;
		m_aPeekedToken = pState->m_aPeekedToken;
		m_aParsedTokenIter = pState->m_aParsedTokenIter;

		return DeleteState( pState );
	}
	return false;
}

bool minTokenizer::DeleteState( minTokenizerState * pState )
{
	if( pState )
	{
		delete pState;
		return true;
	}
	return false;
}

void minTokenizer::AddToken( const minToken & aNewTokenIn )	
{ 
#ifdef _with_fast_token_search
	TokenContainerT::iterator aFoundPos = upper_bound( m_aTokenContainer.begin(), m_aTokenContainer.end(), aNewTokenIn );
	m_aTokenContainer.insert( aFoundPos, aNewTokenIn );
#else
	m_aTokenContainer.push_back( aNewTokenIn ); 
#endif
}

bool minTokenizer::ReadChar()
{
	if( m_sParserString.size()>0 )
	{
		m_sParserString.erase( 0, 1 ); 	// erase
		return true;
	}
	return false;
}

bool minTokenizer::ReadChar( char & ch )
{
	if( m_sParserString.size()>0 )
	{
		ch = m_sParserString[0];
		m_sParserString.erase( 0, 1 );		// erase
		return true;
	}
	return false;
}

// liefert false, falls Zeichen NICHT gefunden, ansonsten true und das Zeichen wird gelesen
bool minTokenizer::ReadCharIfFound( char ch )
{
	/*
	int nPos = m_sParserString.find( ch, 0 );
	if( nPos == 0 )
	{
		m_sParserString.erase( 0, 1 );		// erase
		return true;
	}
	return false;		// nicht gefunden
	*/
	// bessere Performance am 18.1.2000
	if( m_sParserString.length()>0 && ch == m_sParserString[0] )
	{
		m_sParserString.erase( 0, 1 );		// erase
		return true;
	}
	return false;		// nicht gefunden
}

bool minTokenizer::ReadStringIfFound( const string & sString )
{
	size_t nPos = m_sParserString.find( sString.c_str(), 0, sString.size() );
	if( nPos == 0 )
	{
		m_sParserString.erase( 0, sString.size() );	// erase
		return true;
	}
	return false;
}

inline char IsHexChar( char ch )
{
	if( ch>='a' && ch<='f' )
	{
		return true;
	}
	if( ch>='A' && ch<='F' )
	{
		return true;
	}
	return false;
}

char minTokenizer::ReadDigitIfFound( bool bIsHex )
{
#ifdef _with_digit_container
	CharContainerT::const_iterator aIter = m_aDigitContainer.begin();

	while( aIter != m_aDigitContainer.end() )
	{
		if( ReadCharIfFound( *aIter ) )
			return *aIter;
		++aIter;
	}
#else
	// bessere Performance am 18.1.2000
	if( m_sParserString.length()>0 )
	{
		char ch = m_sParserString[0];
		if( (ch>='0' && ch<='9') || (bIsHex && IsHexChar( ch )) )
		{
			m_sParserString.erase( 0, 1 );	// erase
			return ch;
		}
	}
#endif
	return 0;			// keine guelige Ziffer gefunden
}

char minTokenizer::ReadIdentifierCharIfFound()
{
#ifdef _with_digit_container
	CharContainerT::const_iterator aIter = m_aIdentifierCharContainer.begin();

	while( aIter != m_aIdentifierCharContainer.end() )
	{
		if( ReadCharIfFound( *aIter ) )
			return *aIter;
		++aIter;
	}
#else
	// bessere Performance am 18.1.2000
	if( m_sParserString.length()>0 )
	{
		char ch = m_sParserString[0];
		if( (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch=='_' )
		{
			m_sParserString.erase( 0, 1 );	// erase
			return ch;
		}
	}
#endif
	return 0;			// keine guelige Ziffer gefunden
}

bool minTokenizer::IsIdentifierChar( char ch )
{
#ifdef _with_digit_container
	CharContainerT::const_iterator aIter = m_aIdentifierCharContainer.begin();

	while( aIter != m_aIdentifierCharContainer.end() )
	{
		if( *aIter == ch )
			return true;
		++aIter;
	}
#else
	if( (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch=='_' )
	{
		return true;
	}
#endif
	return false;
}

// int: check for (unsigned, short, long) int und (unsigned) short, long
// liefert true, falls das uebergebene Token geaendert wurde !
bool minTokenizer::CheckForIntToken( minToken & aTokenInOut )
{
	minToken aTempToken;
	bool bFound = false;
	if( aTokenInOut.GetId()==UNSIGNED_ID )
	{
		// jetzt darf (nichts), char, int, short oder long folgen

		// Whitespaces ueberlesen
		while( CheckForKnownToken( aTempToken, /*bRead*/false ) && aTempToken.IsWhitespace() )	// Bugfix: 19.1.2000
		{
			CheckForKnownToken( aTempToken, /*bRead*/true );
		}

		if( CheckForKnownToken( aTempToken, /*bRead*/false ) )
		{

			if( aTempToken.GetId()==CHAR_ID )
			{
				aTokenInOut = minToken( Keyword, _UCHAR, UCHAR_ID, m_iLineCount );
				bFound = true;
			}
			else if( aTempToken.GetId()==INT_ID )
			{
				aTokenInOut = minToken( Keyword, _UINT, UINT_ID, m_iLineCount );
				bFound = true;
			}
			else if( aTempToken.GetId()==SHORT_ID )
			{
				aTokenInOut = minToken( Keyword, _USHORT, USHORT_ID, m_iLineCount );
				bFound = true;
			}
			else if( aTempToken.GetId()==LONG_ID )
			{
				aTokenInOut = minToken( Keyword, _ULONG, ULONG_ID, m_iLineCount );
				bFound = true;
			}
		}
		else
		{
			aTokenInOut = minToken( Keyword, _UINT, UINT_ID, m_iLineCount );
			bFound = true;
		}
	}
	else if( aTokenInOut.GetId()==SHORT_ID )
	{
		// jetzt darf (nichts) oder int folgen

		// Whitespaces ueberlesen
		while( CheckForKnownToken( aTempToken, /*bRead*/false ) && aTempToken.IsWhitespace() )
		{
			CheckForKnownToken( aTempToken, /*bRead*/true );
		}

		if( CheckForKnownToken( aTempToken, /*bRead*/false ) )
		{
			if( aTempToken.GetId()==INT_ID )
			{
				//aTokenInOut = minToken( Keyword, _SHORT, SORT_ID, m_iLineCount );
				bFound = true;
			}
		}
	}
	else if( aTokenInOut.GetId()==LONG_ID )
	{
		// jetzt darf (nichts) oder int folgen

		// Whitespaces ueberlesen
		while( CheckForKnownToken( aTempToken, /*bRead*/false ) && aTempToken.IsWhitespace() )
		{
			CheckForKnownToken( aTempToken, /*bRead*/true );
		}

		if( CheckForKnownToken( aTempToken, /*bRead*/false ) )
		{
			if( aTempToken.GetId()==INT_ID )
			{
				//aTokenInOut = minToken( Keyword, _LONG, LONG_ID, m_iLineCount );
				bFound = true;
			}
		}
	}

	if( bFound )
	{
		// gefundenes int-Token auch lesen
		CheckForKnownToken( aTempToken, /*bRead*/true );
		return true;
	}

	return false;
}

bool minTokenizer::CheckForKnownTokenReplaceInt( minToken & aTokenOut )
{
	if( CheckForKnownToken( aTokenOut, /*bRead*/true ) )
	{
		// Spezialbehandlung fuer Integer-Tokens durchfuehren
		if( aTokenOut.IsKeyword() )
			CheckForIntToken( aTokenOut );
		return true;
	}
	return false;
}

#ifdef _slow_search_

bool minTokenizer::CheckForKnownToken( minToken & aTokenOut, bool bRead )
{
	TokenContainerT::const_iterator aIter = m_aTokenContainer.begin();

	while( aIter != m_aTokenContainer.end() )
	{
		const minToken &	aTempToken	= *aIter;
		const string &		sTemp		= aTempToken.GetString();
// TODO gulpxyz: der folgende Aufruf ist teuer ! Besser: optimierte Suche !
		int					nPos		= m_sParserString.find( sTemp.c_str(), 0, sTemp.size() );
		// BEMERKUNG: BUG bis 5.6.1999 ! 
		//   Falls der gefundene Teilstring ein Keyword ist, dann darf das auf den Teilstring folgende
		//   Zeichen KEIN Buchstabe, Ziffer, Underscore sein (d.h. der Zeilstring darf nicht weitergehen) !!!
		//	 Beispiel: string interp;	hier wuerde das interp als int und erp aufgeloest werden !!!
		// Bugfix 6.1.2000: Gleiches gilt fuer 'spezial' Operatoren als Keyword, z.B. new, delete

		if( nPos == 0 )
		{
			if( (aTempToken.IsKeyword() || aTempToken.IsKeywordAsOperator()) && m_sParserString.size()>sTemp.size() && IsIdentifierChar( m_sParserString[sTemp.size()] ) )
			{
				// die Token-Hypothese ist ein Keyword und der Keyword-String geht noch weiter !
				// d.h. das Token ist noch nicht zuende !
			}
			else
			{
				aTokenOut = aTempToken;
				// nur lesen wenn gewuenscht !
				if( bRead )
					m_sParserString.erase( 0, sTemp.size() );	//erase // Arbeitsstring aktualisieren
				return true;
			}
		}
		++aIter;
	}
	return false;
}

#else

// less than ?
class CompareTokensFirstChar
{
public:
	// < Operator neu definieren fuer diesen Vergleich
	bool operator()( const minToken & aLeft, const minToken & aRight ) const 
	{
		// Tokens MUESSEN immer mindestens ein Zeichen besitzen,
		// ansonsten waeren sie keine Tokens !
		return aLeft.GetString()[0] < aRight.GetString()[0];
	}
};

// WICHTIG: der Token-Kontainer muss sortiert sein !!!
bool minTokenizer::CheckForKnownToken( minToken & aTokenOut, bool bRead )
{
	// erzeuge ein Vergleichstoken
	minToken aToken( SpecialChar, m_sParserString.substr( 0, 1 ), 0, m_iLineCount );
	// fuehre binaere Suche im sortierten Token-Kontainer durch, 
	// suche vorlaeufig nur nach dem ersten Zeichen im Token !!!
	pair<TokenContainerT::iterator,TokenContainerT::iterator> aFound 
			= equal_range( m_aTokenContainer.begin(), m_aTokenContainer.end(), aToken, CompareTokensFirstChar() );

	// suche von hinten nach vorne, damit z.B. "++" vor "+" gefunden und bearbeitet wird
	// allerdings steht "+" vor "++" in der sortierten Token-Liste
	TokenContainerT::iterator aIter = aFound.second;

	while( aIter != aFound.first )
	{
		--aIter;
		//cout << "**>" << (*aIter).GetRepresentationString().c_str() << "<" << endl;

		const minToken &	aTempToken	= *aIter;
		const string &		sTemp		= aTempToken.GetString();
		size_t				nPos		= m_sParserString.find( sTemp.c_str(), 0, sTemp.size() );
		// BEMERKUNG: BUG bis 5.6.1999 ! 
		//   Falls der gefundene Teilstring ein Keyword ist, dann darf das auf den Teilstring folgende
		//   Zeichen KEIN Buchstabe, Ziffer, Underscore sein (d.h. der Zeilstring darf nicht weitergehen) !!!
		//	 Beispiel: string interp;	hier wuerde das interp als int und erp aufgeloest werden !!!
		// Bugfix 6.1.2000: Gleiches gilt fuer 'spezial' Operatoren als Keyword, z.B. new, delete

		if( nPos == 0 )
		{
			if( (aTempToken.IsKeyword() || aTempToken.IsKeywordAsOperator()) && m_sParserString.size()>sTemp.size() && IsIdentifierChar( m_sParserString[sTemp.size()] ) )
			{
				// die Token-Hypothese ist ein Keyword und der Keyword-String geht noch weiter !
				// d.h. das Token ist noch nicht zuende !
			}
			else
			{
				aTokenOut = minToken( aTempToken.GetType(), aTempToken.GetString(), aTempToken.GetId(), m_iLineCount );
				// nur lesen wenn gewuenscht !
				if( bRead )
				{
					m_sParserString.erase( 0, sTemp.size() );	//erase // Arbeitsstring aktualisieren
				}
				return true;
			}
		}
	}
	return false;
}

#endif

bool minTokenizer::CheckForNumberToken( minToken & aTokenOut )
{
	// Format: [-][+]{0..9}[.[{0..9}}][e|E[-][+]{0..9}]
	// Format: 0x{0..9,A..F}...

	// schaue nach ob eine gueltige Zahl kommt und konvertiere diese mit der RT-Lib
	string	sTemp	= m_sParserString;
	size_t	nSum	= 0;
	bool	bIsInt	= true;

	// erst mal das Vorzeichen probieren
	if( ReadCharIfFound( '-' ) || ReadCharIfFound( '+' ) )		// Achtung: log. Shortcut wird verwendet !
	{
		// '-' oder '+' gefunden
		nSum++;
	}

	// Bugfix 27.1.2003: hex Zahlen Konstanten sind nun auch erlaubt !
	if( ReadStringIfFound( g_sHexStart ) )
	{
		nSum += strlen( g_sHexStart );	

		// lese eine Hex-Zahl ein...
		char nDigit;
		bool bFoundAnyDigit = false;
		do { 
			nDigit = ReadDigitIfFound( /*bIsHex*/true );
			if( nDigit != 0 )
			{
				bFoundAnyDigit = true;			// Bugfix: 3.1.2000; werden ueberhaupt Ziffern gefunden ?
				nSum++;
			}
		} while( nDigit != 0 );
	}
	else
	{
		// lese eine Dezimal-Zahl ein

		// jetzt die Ziffern vor dem Komma einlesen
		char nDigit;
		bool bFoundAnyDigit = false;
		do { 
			nDigit = ReadDigitIfFound();
			if( nDigit != 0 )
			{
				bFoundAnyDigit = true;			// Bugfix: 3.1.2000; werden ueberhaupt Ziffern gefunden ?
				nSum++;
			}
		} while( nDigit != 0 );

		// kommt jetzt ein Komma ?
		if( ReadCharIfFound( '.' ) )
		{
			// ja --> Ziffern nach dem Komma einlesen
			bIsInt = false;	// Integers besitzen keinen Dezimalpunkt !
			nSum++;
			do { 
				nDigit = ReadDigitIfFound();
				if( nDigit != 0 )
				{
					bFoundAnyDigit = true;		// wenigstens wurden jetzt Nachkomma-Stellen gefunden
					nSum++;
				}
			} while( nDigit != 0 );
		}

		// falls bis hierher keine Ziffern gefunden wurden, dann kann auch kein Exponent kommen ! (3.1.2000)
		if( !bFoundAnyDigit )
			return false;

		// kommt jetzt ggf. ein Exponent ?
		if( ReadCharIfFound( 'e' ) || ReadCharIfFound( 'E' ) )		// Achtung: log. Shortcut wird verwendet !
		{
			// Exponenten Parsen...	
			bIsInt = false;	// Integers besitzen keinen Exponent !
			nSum++;
			// erst mal das Vorzeichen probieren
			if( ReadCharIfFound( '-' ) || ReadCharIfFound( '+' ) )	// Achtung: log. Shortcut wird verwendet !
			{
				// '-' gefunden
				nSum++;
			}

			do { 
				nDigit = ReadDigitIfFound();
				if( nDigit != 0 )
					nSum++;
			} while( nDigit != 0 );
		}

	}

	if( nSum>0 )
	{
		string sNumberString = string( sTemp, 0, nSum );
		//istrstream aStrStream( sNumberString.c_str() );
		//double dVal;
		//aStrStream >> dVal;
		aTokenOut = minToken( Constant, sNumberString, (bIsInt ? INT_NUMBER_ID : FLOAT_NUMBER_ID), m_iLineCount );
		return true;
	}

	return false;
}

bool minTokenizer::CheckForCharToken( minToken & aTokenOut )
{
	// Format: '{char}', \ als Spezialzeichen wie in C/C++

	string	sTemp	= m_sParserString;		// ACHTUNG: ggf. teuer !

	if( ReadCharIfFound( '\'' ) )
	{
		char ch;
		if( ReadChar( ch ) )
		{
			// Bugfix 27.1.2003 fuer Escape Char in Character-Konstanten
			bool bEscapeChar = false;

			if( ch == g_chEscapeChar )
			{
				ReadChar( ch );
				bEscapeChar = true;
			}

			if( ReadCharIfFound( '\'' ) )
			{
				string strg;
				if( bEscapeChar )
				{
					strg += g_chEscapeChar;
				}
				strg += ch;
				aTokenOut = minToken( Constant, strg, CHAR_CONST_ID, m_iLineCount );
				return true;
			}
			else
			{
				// Error in Char-Konstante !
				m_bIsError = true;
				m_nErrorCode = ERROR_IN_CHARCONST;
				m_sParserString = sTemp;	// Zustand der Arbeitsvariable wieder herstellen
				return false;				// Fehler im String
			}
		}
	}
	return false;
}

bool minTokenizer::CheckForStringToken( minToken & aTokenOut )
{
	// Format: "{alles}", \ als Spezialzeichen wie in C/C++

	string	sTemp	= m_sParserString;		// ACHTUNG: ggf. teuer !
	int		nSum	= 0;

	// erst mal nach dem String-Start-Zeichen suchen
	if( ReadCharIfFound( '"' ) )
	{
		bool bStringEnd;
		char ch;
		char chOld = 0;

		while( !(bStringEnd=ReadCharIfFound( '"' )) && ReadChar( ch ) )
		{
			nSum++;
			// nach einem Backslash kann ein String-Ende-Zeichen stehen !
			if( (ch == g_chEscapeChar) && (chOld != g_chEscapeChar ) )		// Bugfix 28.1.2003
			{
				if( ReadCharIfFound( '"' ) )
				{
					nSum++;
				}
			}
			chOld = ch;
		}
		if( !bStringEnd )
		{
			m_bIsError = true;
			m_nErrorCode = ERROR_IN_STRING;
			m_sParserString = sTemp;	// Zustand der Arbeitsvariable wieder herstellen
			return false;				// Fehler im String
		}

		string sStringString = string( sTemp, 1, nSum );		// fuehrendes " wird nicht kopiert
		// ersetzte alle Vorkommen von \n etc., falls es erwuenscht ist !
		if( m_bDoReplace )
		{
			ReplaceStringEscapeChars( sStringString );
		}
		aTokenOut = minToken( Constant, sStringString, STRING_CONST_ID, m_iLineCount );

		return true;
	}

	return false;
}

string minTokenizer::GetStringEscapeChar( char ch )
{
	char chRet = ch;

	switch( ch )
	{
		case 'n':
			chRet = 10;
			break;
		case 'r':
			chRet = 13;
			break;
		case 't':
			chRet = 9;
			break;

		default:
			chRet = ch;
	}

	string sRet;
	sRet += chRet;

	return sRet;
}

bool minTokenizer::ReplaceStringEscapeChars( string & sStringInOut ) const
{
	size_t iPos = sStringInOut.find( g_sEscapeChar );
	if( iPos != string::npos )
	{
		while( iPos != string::npos )
		{
			sStringInOut.replace( iPos, 2, string( GetStringEscapeChar( sStringInOut[iPos+1] ) ) );

			iPos = sStringInOut.find( g_sEscapeChar, iPos+1 );	// Bugfix 28.1.2003, Ersetzung von "abc\\" war fehlerhaft
		}
		return true;
	}
	return false;
}

bool minTokenizer::CheckForBoolToken( minToken & aTokenOut )
{
	// Format: "true|false"

	string	sTemp	= m_sParserString;
	//int		nSum	= 0;

	if( ReadStringIfFound( _TRUE ) )
	{
		aTokenOut = minToken( Constant, string( _TRUE ), BOOL_CONST_ID, m_iLineCount );
		return true;
	}
	else if( ReadStringIfFound( _FALSE ) )
	{
		aTokenOut = minToken( Constant, string( _FALSE ), BOOL_CONST_ID, m_iLineCount );
		return true;
	}

	return false;
}

bool minTokenizer::CheckForCommentToken( minToken & aTokenOut )
{
	string	sTemp = m_sParserString;
	int		nSum = 0;

	// C++-Style-Kommentar
	if( ReadStringIfFound( "//" ) )
	{
		nSum += 2;
	//	bool bCommentEnd = false;
		// alles bis zum Ende der Zeile lesen
		char ch;
		//old: while( !(bCommentEnd=(ReadCharIfFound('\n')||ReadCharIfFound('\r'))) && ReadChar() )
		while( ReadChar( ch ) && ch!='\n' && ch!='\r' )
		{
			nSum++;
		}
	//	if( !bCommentEnd )
	//	{
	//		m_bIsError = true;
	//		m_nErrorCode = ERROR_IN_COMMENT;
	//		m_sParserString = sTemp;	// Zustand der Arbeitsvariable wieder herstellen
	//		return false;				// Fehler im Kommentar
	//	}
	//	else
		{
			// das Zeilenende gehoert NICHT zum Kommentar-Token, (Bugfix: 3.1.2000)
			// daher CR/LF wieder zurueck pushen...
			m_sParserString = string( 1, ch ) + m_sParserString;
		}

		string sCommentString = string( sTemp, 0, nSum );		// noch Kommentar-Ende beachten
		aTokenOut = minToken( Comment, sCommentString, m_iLineCount );
	
		return true;
	}
	// C-Style-Kommentar
	else if( ReadStringIfFound( "/*" ) )
	{
		nSum += 2;
		bool bCommentEnd;
		while( !(bCommentEnd=ReadStringIfFound( "*/" )) && ReadChar() )
		{
			nSum++;
		}
		if( !bCommentEnd )
		{
			m_bIsError = true;
			m_nErrorCode = ERROR_IN_COMMENT;
			m_sParserString = sTemp;	// Zustand der Arbeitsvariable wieder herstellen
			return false;				// Fehler im Kommentar
		}

		string sCommentString = string( sTemp, 0, nSum+2 );		// noch Kommentar-Ende beachten
		aTokenOut = minToken( Comment, sCommentString, 0, m_iLineCount );
	
		return true;
	}

	return false;
}

bool minTokenizer::CheckForIdentifierToken( minToken & aTokenOut )
{
	string	sTemp	= m_sParserString;
	int		nSum	= 0;

	if( ReadIdentifierCharIfFound() )
	{
		nSum++;

		while( ReadIdentifierCharIfFound() || ReadDigitIfFound() )
		{
			nSum++;
		}

		string sStringString = string( sTemp, 0, nSum );
		aTokenOut = minToken( Identifier, sStringString, 0, m_iLineCount );

		return true;
	}

	return false;
}

void minTokenizer::SortTokenContainer()
{
	sort( m_aTokenContainer.begin(), m_aTokenContainer.end() );
}


void minTokenizer::DumpTokenContainer( ostream & aStream, const TokenContainerT & aTokenContainer ) const
{
	TokenContainerT::const_iterator aIter = aTokenContainer.begin();
	int iCount = 0;

	while( aIter != aTokenContainer.end() )
	{
		cout << iCount << " >" << (*aIter).GetRepresentationString().c_str() << "< line=" << (*aIter).GetLineNo() << endl;
		++iCount;
		++aIter;
	}
}

