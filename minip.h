/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minip.h,v $
 *
 *  $Revision: 1.2 $
 *
 *	$Log: not supported by cvs2svn $
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
/*
	Konzept fuer einen allgemeinen Compiler/Interpreter 
	fuer eine C/C++-aehnliche Script-Sprache.

    (c) by Michael Neuroth, 1999-2003

	Start:  3. 2.1999
	Stand: 17. 2.2003


	// Die Konkurenz:
	//===============
	http://root.cern.ch/root/Cint.html					cint
	http://home.mweb.co.za/sd/sdonovan/underc.html		underC
	http://www.softintegration.com/						Ch 3.5
	http://www.anarchos.com/eic/						EiC

	Linksammlungen:
	http://www.ddj.com/topics/cpp/compilers/
	http://www.mathtools.net/C++/Interpreters/

	http://librenix.com/?inode=2171						Ch 3.0

	http://www.cppreference.com/						C++ Reference

	// Known Bugs:
	//============
	siehe: changes.txt

	Bemerkungen zur Implementation:
	  Wo immer es geht werden minHandle-Objekte zur automatischen Verwaltung von Zeigern verwendet.
	  Die minInterpreterNode's und alle davon abgeleiteten Klassen verwalten ihre Sub-Knoten selbst,
	  wenn es nur einzelne Konten sind (z.B. minWhileNode). Muessen die InterpreterNode's Listen 
	  von Sub-Knoten verwalten, so verwenden sie Listen von minHandle-Klassen (z.B. minBlockNode, 
	  minFunctionCallNode).

	Design-Entscheidung:
	  FRAGE: Sollen Methoden am Objekt gemerkt werden oder auf globaler Ebene (Klassenname_Funktionsname(...)) ?
		Sie werden auf globaler Ebene gemerkt mit einem Flag ob es eine Methode ist !
	  Konstruktoren werden folgendermassen kodiert: KlassenName_KlassenName
	  Destruktoren werden folgendermassen kodiert: KlassenName_~KlassenName
	
	  Der Typ der Variablen wird nicht zur parse/compile-Zeit bestimmt sondern erst
	  zur Laufzeit. D.h. auch das Name-Mangling beim Funktionsaufruf findet erst
	  kurz vor dem Call statt. Dadurch ist der Parser einfacher, allerdings kann
	  es zu seltsamen (spaeten) Fehlermeldungen kommen.

	  Vorlaeufig wird keine Zeigerarithmetrik unterstuetzt.

    Bemerkungen zu der Template-Implementierung:
      Templates werden als anonyme Typen behandelt, tritt ein solcher anonymer
      Typ waehrend der Abarbeitung des Programms auf, wird der anonyme Typ ueber
      den Call-Stack aufgeloest. Z.z. koennen nur Klassen, d.h. Objekte, Templates
      unterstuetzen, daher kennt jedes Objekt seine Klasse und (neu) seinen Erzeuger
      (new-Operator bzw. Variable-Deklaration) und an diesem Erzeuger haengt die 
      Typ-Info fuer die Template-Typen dran, als StringList-Info 
      (Beispiel: list<int> aIntList;).

	Offene Arbeitspunkte:
		Richtigen Konstruktor und Destruktor fuer class/struct implementieren.
		Richtige Methoden-Aufrufe fuer class/struct implementieren, this implementieren
		Virtuelle Methoden implementieren;
  		(Arrays, z.B. int a[42]; implementieren)

		Dokumentation erzeugen: Konzepte und Implementation (UML-Diagramme etc.)
		FRAGE: implizite Konversion des Datentyps bei einer Zuweisung ? int i; i = 2.345;
			   -> siehe minInterpreterVariable & operator=( const minInterpreterVariable & rOther ), 
			            minVariableNode::Assign()
			Implizite Konversion der Datentypen wird noch nicht vom Codegenerator unterstuetzt,
			  d.h. "ein String"+1.234 funktioniert im Interpreter, aber nicht in C++ !!!
			  16.11.1999 ==> implizite Konversion funktioniert nicht mehr!

		Alle arithmetrischen Operatoren in einer Klasse behandeln
		unaere Operatoren behandeln (z.B. i++;)
		(Funktionsprototypen implementieren, z.B. int f( int x );)
		switch-Anweisung implementieren
		const Ausdruecke implementieren (z.B. const int g_val = 123;)

		(moeglichst alle Container mit Zeigern in Container mit Handle von Zeigern umwandeln, 
		 siehe minVariableDeclarationList)
		weitere Zeiger-Uebergaben auf Handle-Uebergaben umwandeln 
		  (z.B. GetVariable( const string & sName, minInterpreterVariable * & pVariableOut ))

		Typsichere Datentypen, Sprache ist typsicher !!!
		Verbesserung der Datentyp-Konversionen --> Variablen einem festen Datentyp zuordnen
		Richtige Implementation der Native-Funktions-Anbindung
		schoener formatierte C++ Conversion (Bugs ?)
		ggf. Quellcode-Position am InterpreterItem merken

		Debugging Tools, z.B. Stack-Trace bei Fehler !
		Bessere Parser-Fehlerbehandlung (bessere Fehlermeldungen)!

		P-Code Maschine oder Just In Time Compiler implementieren.

		Tcl/Tk Anbindung (IDE ?)

		Testen, testen, testen...

    Done:
  		(Zuweisung an Objekte implementieren (aObj.i = 7))

	Einschraenkungen:
		der Interpreter/Parser ist NICHT multithread sicher!

    Bekannte Probleme:
		Zahlen mit Vorzeichen !

	Das Sript-Modul besteht aus folgenden Dateien:
		dllexport.h			// defines fuer den Export aus/in DLLs
	// Tokenizer
		mintoken.h			// minTokenType, minToken
		mintokenizer.h		// minTokenizer
	// Parser
		minparsernodes.h	// minInterpreterNode und abgeleitete Klassen
		minparser.h			// minParser
	// Interpreter-Environment
		minipenv.h			// minInterpreterValue, minInterpreterVariable, minCallStackItem, minInterpreterEnvironment
	// der Interpreter
		minip.h				// enthält die Haupt-Klasse: minScriptInterpreter

	// die Implementationen
		mintokenizer.cpp
		minparsernodes.cpp
		minparser.cpp
		minipenv.cpp
		minip.cpp

	// das Testprogramm
		miniptest.cpp
*/

#ifndef _MINIP_H
#define _MINIP_H

//*************************************************************************
#include "mstl_string.h"
#include "mstl_list.h"

//*************************************************************************
#include "dllexport.h"
#include "mintokenizer.h"
#include "minparser.h"
#include "minparsernodes.h"
#include "minipenv.h"
#include "minnativehlp.h"

//*************************************************************************

#include <time.h>

inline unsigned long minGetCurrentTickCount()
{
	return (unsigned long)clock();	// siehe auch CLK_TCK und CLOCKS_PER_SEC
}

#ifdef _MSC_VER
#define CLOCKS_PER_SECOND 1000
#endif

inline unsigned long minGetTickCountPerSec()
{
	return (unsigned long)CLOCKS_PER_SEC;
}


//*************************************************************************

// +++ Fehler-Id's des Interpreters +++
const int RUNTIME_ERROR						= 10000;

//*************************************************************************
class MINDLLEXPORT minInterpreterOutputInterface
{
public:
	virtual void Print( const char * sText ) = 0;
	virtual void PrintLn() = 0;
};

//*************************************************************************
class MINDLLEXPORT minScriptInterpreter
{
public:
	minScriptInterpreter( bool bDebug = false );
	~minScriptInterpreter();

	bool Run( const string & sScriptWithPredefs, const string & sScriptIn, minInterpreterValue & aReturnValueOut, unsigned long * pExecutionTime = 0, unsigned long * pParseTime = 0, const minTokenizer::TokenContainerT & aParsedTokens = minTokenizer::TokenContainerT());
	bool ParseOnly( const string & sScriptIn );
	SMALL( bool GenerateCppCode( const string & sScriptIn, string & sCppCodeOut ); )
	bool RunPreProcessor( bool bOnlyPreproc, const string & sScriptIn, string & sPreProcedScriptOut, const StringListT & aIncludeDirList, minTokenizer::TokenContainerT & aParsedTokens );

	// Methode zum registrieren von weiteren Funktionen am Interpreter
	bool LoadModule( const string & sDllNameIn, long * phDll );
	bool UnLoadModule( long hDll );

	// Hilfsmethoden zur automatischen Erzeugung der C++ Stub-Files (Script muss schon geparsed sein !)
	bool GetAllFunctionPrototypes( minParserItemList & aItemListOut );
	bool GetAllClassPrototypes( minParserItemList & aItemListOut );

	SMALL( bool DumpParser( ostream & aStream ); )

	//bool RegisterNativeFcn( minNativeFcnWrapperBaseAdapter * pNewNativeFcn );

	//bool RegisterFunction();
	//bool RegisterVariable();

	int GetErrorCode() const;

	void SetInterpreterOutputInterface( minInterpreterOutputInterface * pOutputInterface );

    void SetDebugModus( bool bDebug );
    bool GetDebugModus() const;
    void SetDbgModus( bool bDebug );
    bool GetDbgModus() const;
    
	void InitRuntimeEnvironment();
	void DumpAllFunctionPrototypes( ostream & aStream ) const;

private:
	bool GetAllNodes( const string & sNodeName, minParserItemList & aItemListOut );
	void InitTokenizer();
	void ShowErrorPosition();
	minInterpreterNode * const GetProgramNodeForScript( const string & sScript );

	// +++ Daten +++
	minInterpreterEnvironment	m_aEnvironment;		// WICHTIG: erst Environment initialisieren, 
													//		dieses muss am laengsten existieren, da aus dem Parser ueber 
													//		minInterpreterNode's (minInterpreterFunctionDeclarationNode) 
													//		ggf. eine Referenz auf das Environment gehalten wird.
	minTokenizer				m_aTokenizer;
	minParser					m_aParser;
	bool						m_bRunOk;
	bool						m_bDebug;
    bool                        m_bDbg;

	minInterpreterOutputInterface *	m_pOutputInterface;	// not an owner !
};

//*************************************************************************
MINDLLEXPORT bool ReadScript( const char * sFileName, string & sScriptOut, const StringListT & aIncludeDirList = StringListT() );
MINDLLEXPORT bool WriteAsciiFile( const char * sFileName, const string & sTextIn );
MINDLLEXPORT bool SplitPath( const char * sPath, string & sDrive, string & sDir, string & sFileName, string & sExt );

void InitDefaultTokenizer( minTokenizer & m_aTokenizer );

#endif
