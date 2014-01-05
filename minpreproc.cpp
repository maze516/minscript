/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minpreproc.cpp,v $
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

#include "minpreproc.h"
#include "minip.h"			// fuer: ReadScript, minInterpreter

#include "mstl_iostream.h"
#include "mstl_algorithm.h"

/* zum Debuggen:
	{
	cout << "this=" << (void *)&m_aDefineSymbolContainer << " GET: >" << sDefineStrgIn << "<" << endl;
	DefineSymbolContainerT::const_iterator aIter = m_aDefineSymbolContainer.begin();
	while( aIter!=m_aDefineSymbolContainer.end() )
	{
		cout << "***DUMP: " << (*aIter).first << " " << (*aIter).second << endl;
		++aIter;
	}
	}
*/

// Hilfsklasse um bei Containern von pair-Elementen nach nur einem Teil des Pairs zu suchen
template <class Typ1, class Typ2>
class _FindInPair
{
public:
	_FindInPair( const pair<Typ1,Typ2> & aCompObj, bool bCompareFirst = true ) 
		: m_bCompareFirst( bCompareFirst ), m_aCompObj( aCompObj )
	{}

	bool operator()( const pair<Typ1,Typ2> & aObj )
	{
		if( m_bCompareFirst )
			return m_aCompObj.first == aObj.first;
		else
			return m_aCompObj.second == aObj.second;
	}

private:
	bool					m_bCompareFirst;
	const pair<Typ1,Typ2>	m_aCompObj;
};

//*************************************************************************

minPreProcessor::minPreProcessor( bool bOnlyPreproc, const minTokenizer & aTokenizer, const string & sScript, const StringListT & aIncludeDirList, minTokenizer::TokenContainerT & aParsedTokenContainer )
	: m_bOk( true ), 
	  m_bOnlyPreproc( bOnlyPreproc ),
	  m_aTokenizer( aTokenizer ),
	  m_aTokenizerHelper( aTokenizer ),
	  m_aIncludeDirList( aIncludeDirList ),
	  m_aParsedTokenContainer( aParsedTokenContainer )
{ 
	if( m_bOnlyPreproc )
	{
		m_aTokenizer.SetReplaceStringEscapesModus( false );
	}
	SetTextAndInit( sScript ); 
}

minPreProcessor::minPreProcessor( const minTokenizer & aTokenizer, const DefineSymbolContainerT & aDefineSymbolContainer, const string & sIncludeFileName, const StringListT & aIncludeDirList, minTokenizer::TokenContainerT & aParsedTokenContainer )
	: m_bOk( true ), 
	  m_aTokenizer( aTokenizer ),
	  m_aIncludeDirList( aIncludeDirList ),
	  m_aParsedTokenContainer( aParsedTokenContainer )
{
	string sScript;

	if( ReadScript( sIncludeFileName.c_str(), sScript, m_aIncludeDirList ) )
	{
		SetTextAndInit( sScript ); 
		m_aDefineSymbolContainer = aDefineSymbolContainer;
	}
	else
	{
		m_bOk = false;
	}
}

bool minPreProcessor::IsProcessingEnabled() const 
{ 
	return m_aIfStateStack.size()==0 || m_aIfStateStack.top().IsEnabled(); 
}

void minPreProcessor::SetTextAndInit( const string & sScript )
{
	m_bOk = true;
	m_aTokenizer.SetText( sScript );
	m_aTokenizer.InitProcessing();
	// Arbeitsvariablen loeschen
	m_aDefineSymbolContainer.erase( m_aDefineSymbolContainer.begin(), m_aDefineSymbolContainer.end() );
	m_aDependencyList.erase( m_aDependencyList.begin(), m_aDependencyList.end() );
	while( !m_aIfStateStack.empty() )
	{
		m_aIfStateStack.pop();
	}
}

bool minPreProcessor::ReadAndProcessIncludeFile( const string & sIncludeFileName, string & sPreProcedScriptOut )
{
	// Dependecy-List aktualisieren
	m_aDependencyList.push_back( sIncludeFileName );	// ggf. make full path !
	// und rekursiven PreProcessor-Aufruf absetzen
	minPreProcessor aRecursiveProcessor( m_aTokenizer, m_aDefineSymbolContainer, sIncludeFileName, m_aIncludeDirList, m_aParsedTokenContainer );	
	if( aRecursiveProcessor.IsOk() )
	{
		string sTemp;
		if( aRecursiveProcessor.GenerateOutput( sTemp ) )
		{
			sPreProcedScriptOut += sTemp;
			// Define-Symbol-Conatiner aus dem rekursiven Aufruf wird der aktuelle Container, 
			// Defines koennten sich ja geaendert haben !
			m_aDefineSymbolContainer = aRecursiveProcessor.GetDefinedSymbols();
			return true;
		}
	}
	else
	{
		cerr << "ERROR: include file " << sIncludeFileName << " not found." << endl;
	}
	return false;
}

bool minPreProcessor::GenerateOutput( string & sPreProcedScriptOut )
{
	sPreProcedScriptOut = "";	// Ausgabe erst mal loeschen

	minToken aToken;
	bool bOk = true;
	while( bOk && m_aTokenizer.GetNextToken( aToken ) )
	{
		if( aToken.IsPreProcessorStart() )
		{
			// skip whitespaces
			if( m_aTokenizer.GetRealToken( aToken ) )
			{
				// z.Z. werden folgende Preprocessor-Direktiven unterstuezt
					//#define name(arg1,...) value	// #, #@, ##, defined(.)
				//#define name value
				//#undef name
				//#include filename
				//#if const-expr
				//defined name bzw. defined(name)
				//#ifdef name
				//#ifndef name
				//#else
				//#elif
				//#endif
				//#error
				//#message

				// fuer const-expr (Integer-Ausdruck) koennen folgende Operatoren verwendet werden:
				//	<, >, !, ==, !=, ||, &&, defined()
				// Klamerung ist moeglich.
				// IMPLEMENTATION: verwende Interpreter als Expr-Parser und setze eine Funktion define() an dem temporaeren Interpreter

				if( aToken.GetString()=="define" )
				{
					bOk = ProcessDefineSymbol();
				}
				else if( aToken.GetString()=="undef" )
				{
					bOk = ProcessUndefSymbol();
				}
				else if( aToken.GetString()=="include" )
				{
					bOk = ProcessInclude( sPreProcedScriptOut );
				}
				else if( aToken.GetString()=="if" )
				{
					bOk = ProcessIf();
				}
				else if( aToken.GetString()=="elif" )
				{
					bOk = ProcessElif();
				}
				else if( aToken.GetString()=="ifdef" )
				{
					bOk = ProcessIfdef( /*bPositiv*/true );
				}
				else if( aToken.GetString()=="ifndef" )
				{
					bOk = ProcessIfdef( /*bPositiv*/false );
				}
				else if( aToken.GetString()=="else" )
				{
					bOk = ProcessElse();
				}
				else if( aToken.GetString()=="endif" )
				{
					bOk = ProcessEndif();
				}
				else if( aToken.GetString()=="error" )
				{
					bOk = ProcessError();
				}
				else if( aToken.GetString()=="message" )
				{
					bOk = ProcessMessage();
				}
				else if( aToken.GetString()=="pragma" )
				{
					bOk = ProcessPragma();
				}
				else
				{
					cerr << "ERROR: bad preprocessor directive found: #" << aToken.GetString() << endl;
					bOk = false;
				}

				// bei einem Fehler die Verarbeitung sofort abbrechen
				if( !bOk )
					break;
			}
		}
		else
		{
			if( IsProcessingEnabled() )
			{
				// ueberpruefe ob eine define-Anweisung stattfinden muss ?
				string sSymbolOut;
// TODO			
				if( ProcessDefineSearch( m_aTokenizer, aToken, sSymbolOut ) )
				{
					//if( m_bOnlyPreproc )
					{
						sPreProcedScriptOut += sSymbolOut;
					}
					// Performance-Optimierung 11.2.2003
					if( sSymbolOut.length()>0 )
					{
						if( sSymbolOut == aToken.GetRepresentationString() )
						{
							m_aParsedTokenContainer.push_back( aToken );
						}
						else
						{
							// das durch define ersetzte Token neu parsen 
							// und ein Token dafuer erzeugen
							m_aTokenizerHelper.InitProcessing();
							m_aTokenizerHelper.SetText( sSymbolOut );
							if( m_aTokenizerHelper.GetNextToken( aToken ) )
							{
								m_aParsedTokenContainer.push_back( aToken );
							}
						}
					}
				}
				else
				{
					//if( m_bOnlyPreproc )
					{
						sPreProcedScriptOut += aToken.GetRepresentationString();
					}
					m_aParsedTokenContainer.push_back( aToken );
				}
			}
		}
	}

	// Bugfix 22.1.2003:
	// am Ende der Verarbeitung noch den Zustand des Preprocessors pruefen
	// vielleicht ist ja ein Fehler aufgetreten, z.B. ein fehlendes #endif
	if( bOk && (m_aIfStateStack.size() != 0) )
	{
		cerr << "ERROR: missing #endif for: ";
		while( !m_aIfStateStack.empty() )
		{
			_IfStateHelper aItem = m_aIfStateStack.top();

			cerr << aItem.GetCondition().c_str();
			
			m_aIfStateStack.pop();
			
			if( !m_aIfStateStack.empty() )
			{
				cerr << ", ";
			}
		}
		cerr << endl;
		bOk = false;
	}

	return bOk;
}

// lese eine Aufruf-Liste einer Define-Funktion: z.B. min_max( 3*4, b )  --> TODO min_max( (a+b) ) Stack fuer Klammern !!!
bool minPreProcessor::ReadActualArgumentTokens( minTokenizer & aTokenizer, const minTokenContainerT & aArgTokenListIn, minTokenContainerConatinerT & aArgContainerOut )
{
	minToken aToken;
	if( aTokenizer.PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
	{
		// ( lesen
		aTokenizer.GetNextToken( aToken );

		// Argumente lesen
		int nParenthisCount = 0;
		minTokenContainerT::const_iterator aArgumentIter = aArgTokenListIn.begin();
		while( aTokenizer.PeekRealToken( aToken ) && !(nParenthisCount==0 && aToken.IsParenthisClose()) && aArgumentIter!=aArgTokenListIn.end() )
		{
			// baue Tokenliste fuer das aktuelle Argument zusammen
			minTokenContainerT aParamList;

			// lese Argument-Ausdruck, wird beendet durch
			while( aTokenizer.PeekNextToken( aToken ) && !(nParenthisCount==0 && (aToken.IsParenthisClose() || (aToken.IsOperator() && aToken.GetId()==COMMA_ID))) )
			//while( aTokenizer.PeekRealToken( aToken ) && !aToken.IsParenthisClose() && !(aToken.IsOperator() && aToken.GetId()==COMMA_ID) )	// (ACHTUNG: Whitespace werden z.Z. ignoriert !!! )
			{
				// 'Spezialbehandlung' fuer Klammer-Schachtelung
				if( aToken.IsParenthisOpen() )
					nParenthisCount++;
				if( aToken.IsParenthisClose() )
					nParenthisCount--;

				aTokenizer.ReadNextToken();
				aParamList.push_back( aToken );
			}
			if( aToken.IsOperator() && aToken.GetId()==COMMA_ID )
			{
				aTokenizer.ReadNextToken();
			}

			// Argument vollstaendig eingelesen, erzeuge Eintrag fuer den Ergebniskontainer
			aArgContainerOut.push_back( pair<minToken,minTokenContainerT>( *aArgumentIter ,aParamList ) );

			++aArgumentIter;
		}
		if( aArgumentIter != aArgTokenListIn.end() )
		{
			// TODO Error... zu wenige aktuelle Parameter
			return false;
		}

		// ) lesen
		if( aToken.IsParenthisClose() )
		{
			aTokenizer.GetNextToken( aToken );
		}
		else
		{
			// TODO Error... ggf. zu viele aktuelle Parameter
			return false;
		}

		return true;
	}
	return false;
}

// lese eine einfache Argument Liste: ( identifier1, identifier2, ... )
// dabei muss der identifier aus einem einzigen Token bestehen, d.h. ( a*b, c ) ist nicht erlaubt!
bool minPreProcessor::ReadArgumentTokens( minTokenContainerT & aArgTokenListOut )
{
	minToken aToken;
	if( m_aTokenizer.PeekRealToken( aToken ) && aToken.IsParenthisOpen() )
	{
		// ( lesen
		m_aTokenizer.GetNextToken( aToken );

		// Argumente lesen
		while( m_aTokenizer.PeekRealToken( aToken ) && !aToken.IsParenthisClose() )
		{
			if( aToken.IsIdentifier() )
			{
				// Identifier lesen
				m_aTokenizer.GetNextToken( aToken );
				aArgTokenListOut.push_back( aToken );

				// jetzt muss ein , oder ein ) folgen
				if( m_aTokenizer.PeekRealToken( aToken ) && aToken.IsOperator() && aToken.GetId()==COMMA_ID )
				{
					// , lesen
					m_aTokenizer.GetNextToken( aToken );
				}
			}
			else
			{
				// TODO Error setzten
				return false;
			}
		}

		// ) lesen
		if( aToken.IsParenthisClose() )
		{
			m_aTokenizer.GetNextToken( aToken );
		}
		else
		{
			// TODO Error...
			return false;
		}

		return true;
	}
	return false;
}

bool minPreProcessor::GetSymbolIfDefined( const string & sDefineStrgIn, string & sSymbolOut ) const
{
/*	debugging: 
	cout << "##################" << endl;
	DefineSymbolContainerT::const_iterator aIterShow = m_aDefineSymbolContainer.begin();
	while( aIterShow != m_aDefineSymbolContainer.end() )
	{
		cout << "-> " << (*aIterShow).first.c_str() << endl;
		++aIterShow;
	}
*/
	DefineSymbolContainerT::const_iterator aIter = m_aDefineSymbolContainer.find( sDefineStrgIn );
	if( aIter!=m_aDefineSymbolContainer.end() )
	{
		sSymbolOut = (*aIter).second;
		return true;
	}
//TODO: richtig implementieren
	// Behandlung von __FILE__, __LINE__, __TIME__ und __DATE__
	if( sDefineStrgIn == "__FILE__" )
	{
		sSymbolOut = "\"TODO: DATEINAME\"";
		return true;
	}
	else if( sDefineStrgIn == "__LINE__" )
	{
		sSymbolOut = "\"TODO: ZEILE_NUMMER\"";
		return true;
	}
	else if( sDefineStrgIn == "__TIME__" )
	{
		sSymbolOut = "\"TODO: ZEITANGABE\"";
		return true;
	}
	else if( sDefineStrgIn == "__DATE__" )
	{
		sSymbolOut = "\"TODO: DATUMSANGABE\"";
		return true;
	}
	return false;
}

bool minPreProcessor::DeleteSymbolIfDefined( const string & sDefineStrgIn )
{
	DefineSymbolContainerT::iterator aIter = m_aDefineSymbolContainer.find( sDefineStrgIn );
	if( aIter!=m_aDefineSymbolContainer.end() )
	{
		m_aDefineSymbolContainer.erase( aIter );
		return true;
	}
	return false;
}

bool minPreProcessor::IsDefineFunction( const string & sDefineStrgIn ) const
{
	DefineFunctionContainerT::const_iterator aIter = m_aDefineFunctionContainer.find( sDefineStrgIn );
	return aIter!=m_aDefineFunctionContainer.end();
}

string minPreProcessor::RecursiveProcessDefineSerach( const string & sTokenString )
{
	// TODO: Optimierung moeglich: hier wird der Tokenizer kopiert, dies ist teuer !!!
	minTokenizer aTempTokenizer( m_aTokenizer );	// kopiere alle Tokens
	aTempTokenizer.SetText( sTokenString );

	// zerlege den gefundenen Ersatz-Define-String in Tokens und 
	// behandele diese Tokens ebenfalls als Define-Ersatz (falls notwendig) --> Rekursion !
	string sOut;
	minToken aTempToken;
	while( aTempTokenizer.GetNextToken( aTempToken ) )
	{
		// rekursiver Aufruf...
		string sTempRecursive;
		// WICHTIG: hier der Methode den temporaeren Tokenizer uebergeben !!! (19.1.2000)
		if( ProcessDefineSearch( aTempTokenizer, aTempToken, sTempRecursive ) )
		{
			sOut += sTempRecursive;
		}
		else
		{
			sOut += aTempToken.GetRepresentationString();	// GetString();
		}
	}
	return sOut;
}

// Verarbeite eine Ersetzung eines durch define definierten Symbols
bool minPreProcessor::ProcessDefineSearch( minTokenizer & aTokenizer, const minToken & aActToken, string & sSymbolOut )
{
	bool bOk = false;
	string sTemp;

	if( aActToken.IsIdentifier() && GetSymbolIfDefined( aActToken.GetString(), sTemp ) )
	{
		sSymbolOut = RecursiveProcessDefineSerach( sTemp );
		return true;
	}
	else if( aActToken.IsIdentifier() && ProcessDefineFunctionIfDefined( aTokenizer, aActToken.GetString(), sTemp, bOk ) )
	{					 
		sSymbolOut = RecursiveProcessDefineSerach( sTemp );
		return true;
	}
	else
	{
		sSymbolOut = aActToken.GetRepresentationString();
		return false;
	}
	return false;
}

// #define wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessDefineSymbol()
{
	// Unterscheidung zwischen Fall a) "#define a b" und zwischen Fall b) "#define a(b,c) (b*c)" notwendig

	// Fall b) expand arguments before substitution !

	// vorlaeufig darf nur ein String/Identifier-Token (Name) folgen und 
	// der Rest der Zeile gehoert zum Ersetzungsstring
	// ACHTUNG: es werden noch keine \-Tokens fuer Fortsetzung der Zeile in die naechste Zeile unterstuetzt

	// Define Symbol lesen dabei: skip whitespaces
	minToken aToken;
	if( m_aTokenizer.GetRealToken( aToken, /*bIsNewLineWhitespace*/false ) && aToken.IsIdentifier() )
	{
		string sDefineStrg = aToken.GetString();

		m_aTokenizer.PeekNextToken( aToken );

		// Fallunterscheidung durchfuehren
		if( aToken.IsParenthisOpen() )
		{
			// Fall b) behandeln (6.1.2000)

			minTokenContainerT aArgList;
			if( !ReadArgumentTokens( aArgList ) )
			{
				// Error parsing Arguments
				return false;
			}

			// skip whitespaces
			// Bugfix 22.1.2003, aber keine New-Line ueberlesen, dieses Token beendet #define !
			m_aTokenizer.PeekRealToken( aToken, /*bIsNewLineWhitespace*/false );

			// jetzt die zu ersetzenden Tokens lesen
			minTokenContainerT aReplacementList;
			bool bContinue = true;
			while( bContinue && m_aTokenizer.GetNextToken( aToken ) )
			{
				if( aToken.IsBackslash() )
				{
					// jetzt darf nur noch ein NewLine folgen...
					if( m_aTokenizer.GetNextToken( aToken ) && !aToken.IsNewLine() )
					{
						// TODO Error---> nach \ darf nichts mehr folgen
						return false;
					}
				}
				else
				{
					bContinue = !aToken.IsNewLine();
				}
				// das New-Line gehoert nicht zur Ersetzung dazu
				if( bContinue )
				{
					aReplacementList.push_back( aToken );
				}
			}

			// nur Symbol anlegen, wenn die Verarbeitung enabled ist
			if( IsProcessingEnabled() )
			{
				if( !IsDefineFunction( sDefineStrg ) )
				{
					m_aDefineFunctionContainer[ sDefineStrg ] = _DefineFunctionHelper( aArgList, aReplacementList );
				}
				else
				{
					cerr << "WARNING: " << sDefineStrg.c_str() << " allready defined as " 
						 << m_aDefineFunctionContainer[ sDefineStrg ].GetReplacementString().c_str() 
						 << " now redifined as ";
					m_aDefineFunctionContainer[ sDefineStrg ] = _DefineFunctionHelper( aArgList, aReplacementList );
					cerr  << m_aDefineFunctionContainer[ sDefineStrg ].GetReplacementString().c_str() << endl;
				}
			}
		}
		else
		{
			// Fall a) behandeln

			// whitespaces nicht ignorieren, damit "#define a" korrekt behandelt wird (16.1.2000)
			m_aTokenizer.PeekRealToken( aToken, /*bIsNewLineWhitespace*/false );
			string sReplaceStrg;
			if( aToken.IsNewLine() )
			{
				// replace String ist leer
			}
			else
			{
				// Replace String einlesen
// TODO gulpxyz: hier auch eine Liste von Tokens speichern, damit spaeter rekursiv mit der Ersetzung aufgesetzt werden kann ! --> obsolet durch rekursiven Tokenizer Aufruf
				while( m_aTokenizer.GetNextToken( aToken ) && !aToken.IsNewLine() )
				{
					sReplaceStrg += aToken.GetRepresentationString();
				}
			}

			// nur Symbol anlegen, wenn die Verarbeitung enabled ist
			if( IsProcessingEnabled() )
			{
				// falls Schluessel noch nicht existiert hinzufuegen
				if( m_aDefineSymbolContainer.find( sDefineStrg )==m_aDefineSymbolContainer.end() )
				{
					m_aDefineSymbolContainer[ sDefineStrg ] = sReplaceStrg;
				}
				else
				{
					// Warnung ausgeben, falls schon vorhanden !
					cerr << "WARNING: " << sDefineStrg.c_str() << " allready defined as " << m_aDefineSymbolContainer[ sDefineStrg ].c_str() << 
							" now redifined as " << sReplaceStrg.c_str() << endl;
					m_aDefineSymbolContainer[ sDefineStrg ] = sReplaceStrg;
				}
			}
		}
		return true;
	}

	return false;
}

bool minPreProcessor::ProcessDefineFunctionIfDefined( minTokenizer & aTokenizer, const string & sDefineStrgIn, string & sSymbolOut, bool & bOk )
{
	bOk = true;
	DefineFunctionContainerT::const_iterator aIter = m_aDefineFunctionContainer.find( sDefineStrgIn );
	if( aIter!=m_aDefineFunctionContainer.end() )
	{
		const minTokenContainerT & aArgDefList = (*aIter).second.GetArguments();
		const minTokenContainerT & aReplacementDefList = (*aIter).second.GetReplacement();

		// lese Argumente, z.B. ( 1, 3 ) und merge die Aktuellen-Parameter und die Formalen-Parameter in eine Datenstruktur
		minTokenContainerConatinerT aActArgList;
		if( !ReadActualArgumentTokens( aTokenizer, aArgDefList, aActArgList ) )
		{
			// Error parsing Arguments
			bOk = false;
			return false;
		}

		// und fuehre dann die Ersetzung durch
		sSymbolOut = "";
		string sTemp;
		minTokenContainerT::const_iterator aReplIter = aReplacementDefList.begin();
		while( aReplIter != aReplacementDefList.end() )
		{
			// Stringizing Operator # behandeln
			bool bStringizing = false;
			if( (*aReplIter).IsPreProcessorStart() )
			{
				// Stringizing einschalten und naechstes Token holen
				bStringizing = true;
				++aReplIter;
				if( aReplIter == aReplacementDefList.end() )
				{
					// TODO Error Token nach # erwartet
					return true;
				}
			}
			// Token Pasting Operator ## behandeln
			if( (*aReplIter).IsPreProcConcatenate() )
			{
				// Token einfach ignorieren !
				++aReplIter;
				if( aReplIter == aReplacementDefList.end() )
				{
					// TODO Error Token nach ## erwartet
					return true;
				}
			}

			if( bStringizing )
			{
				sSymbolOut += "\"";
			}
			// ist der Token im Replacement-String zu ersetzten:
			// suche im normalen Define-Kontainer und in der Argument-Liste
// TODO gulpxyz: hier auch nach Define-Funktionen Suchen... (rekursiver Aufruf !)
			if( GetSymbolIfDefined( (*aReplIter).GetString(), sTemp ) )
			{
				sSymbolOut += sTemp;
			}
			else 
			{
				// ist es ein formales Argument im Ersetzungs-String ?
				minTokenContainerConatinerT::const_iterator aFoundIter = find_if( aActArgList.begin(), aActArgList.end(), _FindInPair<minToken,minTokenContainerT>( pair<minToken,minTokenContainerT>(*aReplIter,minTokenContainerT()) ) );
				if( aFoundIter != aActArgList.end() )
				{
					// ersetzte Token durch aktuelles Argument
					minTokenContainerT::const_iterator aActArgIter = (*aFoundIter).second.begin();
					while( aActArgIter != (*aFoundIter).second.end() )
					{
						sSymbolOut += (*aActArgIter).GetRepresentationString();		// bis 22.1.2003: GetString()
						++aActArgIter;
					}
				}
				else
				{
					// es ist ein einfaches Token, dass nicht ersetzt werden muss
					sSymbolOut += (*aReplIter).GetRepresentationString();		// bis 22.1.2003: GetString()
				}
			}
			if( bStringizing )
				sSymbolOut += "\"";

			++aReplIter;
		}

		return true;
	}

	return false;
}

// #undef wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessUndefSymbol()
{
	// es darf nur ein String/Identifier-Token kommen, dieses wird dann als Symbol geloescht

	minToken aToken;
	if( m_aTokenizer.GetRealToken( aToken, /*bIsNewLineWhitespace*/false ) && aToken.IsIdentifier() )
	{
		// nur Symbol loeschen, wenn die Verarbeitung enabled ist
		if( IsProcessingEnabled() )
		{
			DeleteSymbolIfDefined( aToken.GetString() );
		}
		return true;
	}

	return false;
}

// #include wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessInclude( string & sPreProcedScriptOut )
{
	// skip whitespaces
	minToken aToken;
	if( m_aTokenizer.GetRealToken( aToken, /*bIsNewLineWhitespace*/false ) )
	{
		// Include-Anweisungen koennen sein:
		//	a) #include <stdio.h>
		//	b) #include "myheader.h"

		// behandle Fall a)
		if( aToken.IsOperator() && aToken.GetId()==LESS_ID )
		{
			string sIncludeName;

			while( m_aTokenizer.PeekNextToken( aToken ) && !(aToken.IsOperator() && aToken.GetId()==MORE_ID) )
			{
				m_aTokenizer.GetNextToken( aToken );
				sIncludeName += aToken.GetString();
			}
			if( m_aTokenizer.GetRealToken( aToken ) && aToken.IsOperator() && aToken.GetId()==MORE_ID )
			{
				// nur include durchfuehren, wenn die Verarbeitung enabled ist
				if( IsProcessingEnabled() )
				{
					// PreProcessor rekursiv aufrufen...
					return ReadAndProcessIncludeFile( sIncludeName, sPreProcedScriptOut );
				}
				else
				{
					// ueberlese include Datei, da processing gerade nicht eingeschaltet ist
					// ggf. wegen einer #ifdef Direktive!
					return true;
				}
			}
		}
		// behandle Fall b)
		else if( aToken.IsString() )
		{
			// nur include durchfuehren, wenn die Verarbeitung enabled ist
			if( IsProcessingEnabled() )
			{
				// PreProcessor rekursiv aufrufen...
				return ReadAndProcessIncludeFile( aToken.GetString(), sPreProcedScriptOut );
			}
			else
			{
				// ueberlese include Datei, da processing gerade nicht eingeschaltet ist
				// ggf. wegen einer #ifdef Direktive!
				return true;
			}
		}

		// Fehler bei der Include-Direktive aufgetreten
		return false;
	}

	return false;
}

// Hilfsfunktion zur Ueberpruefung ob ein String eine gueltige Zahl enthaelt
bool ContainsStringANumber( const string & sValueStrg, long * plNumber )
{
	long nTempNumber = atol( sValueStrg.c_str() );
	bool bIsNumber = (nTempNumber != 0) || ( (sValueStrg.length()>0) && (sValueStrg[0]=='0') );
	
	// nur falls 0 geliefert wird muss der String nochmal separat geprueft werden...
	if( nTempNumber==0 )
	{
		if( sValueStrg=="0" || sValueStrg=="0.0" )
			bIsNumber = true;
	}

	if( bIsNumber && plNumber )
		*plNumber = nTempNumber;

	return bIsNumber;
}

// #if wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessIf( bool bDoEvaluation )
{
	// fuer den einfachen Integer-Ausdruck einfach den eigenen Interpreter zur Auswertung verwenden !!!
	// Rufe den Interpreter auf um den Ausdruck auszuwerten, vorher
	// alle Defines als String anlegen
	// bool bCondition = (#if-Ausdruck);
	// bCondition;
	//string sTempScript = "{\n bool defined( string sSymbol ) { PrintLn( \"defined \"+sSymbol ); return _*exists sSymbol ; }\n";
	string sTempScript = "{\n";

	// define-Symbole anlegen
	DefineSymbolContainerT::const_iterator aIter = m_aDefineSymbolContainer.begin();
	while( aIter != m_aDefineSymbolContainer.end() )
	{
		const string & sValue = (*aIter).second; 
		long nTempNumber = 0;
		bool bIsNumber = ContainsStringANumber( sValue, &nTempNumber );

		if( bIsNumber )
			sTempScript += "int ";
		else
			sTempScript += "string ";

		sTempScript += (*aIter).first + " = ";

		// falls notwendig noch die String-Konstanten mit "" versehen
		if( bIsNumber || (sValue.size()>0 && sValue.at(0)=='"') )
			sTempScript += sValue;
		else
			sTempScript += "\"" + sValue + "\"";

		sTempScript += ";\n";
		++aIter;
	}

	// Code fuer Bedingung erzeugen
	sTempScript += "bool bCondition = ( ";
	string sConditionStrg;
	minToken aToken;
	while( m_aTokenizer.GetNextToken( aToken ) && !aToken.IsNewLine() )
	{
		sConditionStrg += aToken.GetString();
	}
	sTempScript += sConditionStrg + " );\nbCondition;\n}";

	if( bDoEvaluation )
	{
		minScriptInterpreter aIp;
		minInterpreterValue aReturnValue;
		if( aIp.Run( sTempScript, aReturnValue ) )
		{
			if( aReturnValue.GetBool() )
			{
				// Bedingung erfuellt --> then-Teil ausfuehren
				m_aIfStateStack.push( _IfStateHelper( /*bIsThen*/true, /*bIsEnabled*/true, sConditionStrg ) );
			}
			else
			{
				// Bedingung NICHT erfuellt --> else-Teil ausfuehren, d.h. then-Teil disablen
				m_aIfStateStack.push( _IfStateHelper( /*bIsThen*/true, /*bIsEnabled*/false, sConditionStrg ) );
			}
			return true;
		}
	}
	else
	{
		// nur die Argumente von #elif wegparsen und mit Erfolg zurueckkehren
		return true;
	}

	return false;
}

// #elif wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessElif()
{
	ProcessElse();
	// muss der else-Zweig ueberhaupt verarbeitet werden 
	// oder war der then-Zweig aktiv ?
	if( IsProcessingEnabled() )
	{
		// JA verarbeiten
		ProcessEndif();			// alten else-Teil ausschalten (d.h. vom If-Stack holen)
		return ProcessIf();		// und neuen If-Teil auswerten
	}
	else
	{
		// NEIN einfach Argumente von #elif parsen und diese und den Block ignorieren
		return ProcessIf( /*bDoEvaluation*/false );
	}
	return true;
}

// #ifdef bzw. #ifndef wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessIfdef( bool bPositiv )
{
	// nun darf ein String-Token folgen
	// skip whitespaces
	minToken aToken;
	if( m_aTokenizer.GetRealToken( aToken, /*bIsNewLineWhitespace*/false ) && (aToken.IsIdentifier() || aToken.IsConstant()))
	{
		if( aToken.IsIdentifier() )
		{
			// suche das angegebene Symbol in dem Container
			string sSymbolOut;
			if( GetSymbolIfDefined( aToken.GetString(), sSymbolOut ) )
			{
				// gefunden --> then-Teil ausfuehren
				m_aIfStateStack.push( _IfStateHelper( /*bIsThen*/true, /*bIsEnabled*/(bPositiv ? true : false), aToken.GetString() ) );
			}
			else
			{
				// NICHT gefunden --> ggf. else-Teil ausfuehren, d.h. then-Teil disablen
				m_aIfStateStack.push( _IfStateHelper( /*bIsThen*/true, /*bIsEnabled*/(bPositiv ? false : true), aToken.GetString() ) );
			}
		}
		else
		{
			// bei #ifdef muss ein Identifier stehen, eine Konstante ist NICHT erlaubt
			cerr << "ERROR: need identifier for #ifdef" << endl;
			return false;
		}
		return true;
	}

	return false;
}

// #else wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessElse()
{
	if( m_aIfStateStack.size()>0 )
	{
		// Bugfix 22.1.2003: nur wenn wir noch nicht im else-Modus sind auch umschalten !
		if( !m_aIfStateStack.top().IsElse() )
		{
			return m_aIfStateStack.top().SwitchToElse();
		}
		else
		{
			return true;
		}
	}
	return false;
}

// #endif wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessEndif()
{
	if( m_aIfStateStack.size()>0 )
	{
		m_aIfStateStack.pop();
		return true;
	}
	return false;
}

// #error wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessError()
{
	if( IsProcessingEnabled() )
	{
		string sErrorStrg;
		minToken aToken;
		while( m_aTokenizer.GetRealToken( aToken, /*bIsNewLineWhitespace*/false ) && !aToken.IsNewLine() )
		{
			sErrorStrg += aToken.GetString();
		}

		cerr << "#error directive found: " << sErrorStrg.c_str() << endl;
	}
	return false;		// Verarbeitung immer abbrechen
}

// #message wurde vor dem Aufruf schon gelesen
bool minPreProcessor::ProcessMessage()
{
	if( IsProcessingEnabled() )
	{
		string sMessageStrg;
		minToken aToken;
		while( m_aTokenizer.GetRealToken( aToken, /*bIsNewLineWhitespace*/false ) && !aToken.IsNewLine() )
		{
			sMessageStrg += aToken.GetString();
		}

		cerr << "#message: " << sMessageStrg.c_str() << endl;
	}
	return true;
}

// behandele #paragma Anweisungen momentan wie #mesage Anweisungen
bool minPreProcessor::ProcessPragma()
{
	if( IsProcessingEnabled() )
	{
		string sMessageStrg;
		minToken aToken;
		while( m_aTokenizer.GetRealToken( aToken, /*bIsNewLineWhitespace*/false ) && !aToken.IsNewLine() )
		{
			sMessageStrg += aToken.GetString();
		}

		cerr << "#pragma: " << sMessageStrg.c_str() << endl;
	}
	return true;
}

