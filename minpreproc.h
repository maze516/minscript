/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minpreproc.h,v $
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

#ifndef _MINPREPROC_H
#define _MINPREPROC_H

#include "dllexport.h"
#include "mintoken.h"
#include "mintokenizer.h"

#include "mstl_string.h"
#include "mstl_map.h"
#include "mstl_list.h"
#include "mstl_stack.h"

//*************************************************************************
typedef list<minToken>								minTokenContainerT;
typedef list< pair<minToken,minTokenContainerT> >	minTokenContainerConatinerT;

//*************************************************************************
class _IfStateHelper
{
public:
	_IfStateHelper( bool bIsThen = true, bool bIsEnabled = true, const string & sName = "" )
		: m_bIsThen( bIsThen ), 
		  m_bIsEnabled( bIsEnabled ),
		  m_sName( sName )
	{}

	bool IsThen() const			{ return m_bIsThen; }
	bool IsElse() const			{ return !IsThen(); }
	bool IsEnabled() const		{ return m_bIsEnabled; }

	bool SwitchToElse()
	{
		if( m_bIsThen )
		{
			// beim Switch in den else-Zweig muss auch der
			// Zustand der Verarbeitung umgeschaltet werden !
			m_bIsThen = false;
			m_bIsEnabled = !m_bIsEnabled;
			return true;
		}
		return false;
	}

	string GetCondition() const	{ return m_sName; }

#ifdef _MSC_VER
	// Dummy-Implementationen
	bool operator==( const _IfStateHelper & aOther ) const	{ return this == &aOther; }
	bool operator!=( const _IfStateHelper & aOther ) const	{ return !(*this==aOther); }
	bool operator<( const _IfStateHelper & aOther ) const	{ return false; }
	bool operator>( const _IfStateHelper & aOther ) const	{ return false; }
#endif

private:
	bool	m_bIsThen;		// true==then-Zweig, false==else-Zweig
	bool	m_bIsEnabled;
	string	m_sName;
};

//*************************************************************************
class _DefineFunctionHelper
{
public:
	_DefineFunctionHelper( const minTokenContainerT & aArguments = minTokenContainerT(), const minTokenContainerT & aReplacement = minTokenContainerT() )
		: m_aArguments( aArguments ), m_aReplacement( aReplacement )
	{}

	const minTokenContainerT &	GetArguments() const	{ return m_aArguments; }	
	const minTokenContainerT &	GetReplacement() const	{ return m_aReplacement; }	

	string	GetRepresentationString( const minTokenContainerT & aContainer ) const
	{
		string sRet;

		minTokenContainerT::const_iterator aIter = aContainer.begin();
		while( aIter != aContainer.end() )
		{
			sRet += (*aIter).GetRepresentationString();
			++aIter;
		}

		return sRet;
	}

	string	GetReplacementString() const
	{
		return GetRepresentationString( m_aReplacement );
	}

private:
	minTokenContainerT			m_aArguments;
	minTokenContainerT			m_aReplacement;
};

//*************************************************************************
class MINDLLEXPORT minPreProcessor
{
	typedef map<string,string,less<string> >					DefineSymbolContainerT;     // PALM
	typedef map<string,_DefineFunctionHelper,less<string> > 	DefineFunctionContainerT;   // PALM
	typedef stack<_IfStateHelper>								IfStateStackT;

public:
	minPreProcessor( bool bOnlyPreproc, const minTokenizer & aTokenizer, const string & sScript, const StringListT & aIncludeDirList, minTokenizer::TokenContainerT & aParsedTokenContainer );

	bool GenerateOutput( string & sPreProcedScriptOut );

	const DefineSymbolContainerT & GetDefinedSymbols() const	{ return m_aDefineSymbolContainer; }

	bool IsOk() const											{ return m_bOk; }

private:
	// Konstruktor fuer rekursiven Aufruf...
	minPreProcessor( const minTokenizer & aTokenizer, const DefineSymbolContainerT & aDefineSymbolContainer, const string & sIncludeFileName, const StringListT & aIncludeDirList, minTokenizer::TokenContainerT & aParsedTokenContainer );

	// +++ Hilfsmethoden +++
	bool IsProcessingEnabled() const;

	void SetTextAndInit( const string & sScript );
	bool ReadAndProcessIncludeFile( const string & sIncludeFileName, string & sPreProcedScriptOut );

	bool ReadArgumentTokens( minTokenContainerT & aArgTokenListOut );
	bool ReadActualArgumentTokens( minTokenizer & aTokenizer, const minTokenContainerT & aArgTokenListIn, minTokenContainerConatinerT & aArgContainerOut );
	bool IsDefineFunction( const string & sDefineStrgIn ) const;
	bool GetSymbolIfDefined( const string & sDefineStrgIn, string & sSymbolOut ) const;
	bool DeleteSymbolIfDefined( const string & sDefineStrgIn );

	string RecursiveProcessDefineSerach( const string & sTokenString );
	bool ProcessDefineSearch( minTokenizer & aTokenizer, const minToken & aActToken, string & sSymbolOut );							// Abfrage
	bool ProcessDefineSymbol();																				// Definition
	bool ProcessDefineFunctionIfDefined( minTokenizer & aTokenizer, const string & sDefineStrgIn, string & sSymbolOut, bool & bOk );	// Definition
	bool ProcessUndefSymbol();
	bool ProcessInclude( string & sPreProcedScriptOut );
	bool ProcessIf( bool bDoEvaluation = true );
	bool ProcessElif();
	bool ProcessIfdef( bool bPositiv );
	bool ProcessElse();
	bool ProcessEndif();
	bool ProcessError();
	bool ProcessMessage();
	bool ProcessPragma();

	// +++ Daten +++
	bool						m_bOk;
	bool						m_bOnlyPreproc;
	minTokenizer				m_aTokenizer;
	minTokenizer				m_aTokenizerHelper;
	DefineSymbolContainerT		m_aDefineSymbolContainer;	// hier werden die Define-Anweisungen gespeichert (z.B. #define a b)
	DefineFunctionContainerT	m_aDefineFunctionContainer;	// hier werden die Define-Funktionen gespeichert (z.B. #define f(a,b) (a>b))
	StringListT					m_aIncludeDirList;			// in welchen Verzeichnissen sollen includes gesucht werden
	StringListT					m_aDependencyList;			// von wem haengt dieses Script ab (includes)
	IfStateStackT				m_aIfStateStack;

	minTokenizer::TokenContainerT &	m_aParsedTokenContainer;
};

#endif
