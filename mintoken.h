/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/mintoken.h,v $
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

#ifndef _MINTOKEN_H
#define _MINTOKEN_H

//*************************************************************************
#include "mstl_string.h"

//*************************************************************************
#include "dllexport.h"

//*************************************************************************

extern const char * g_sHexStart;
extern const char * g_sEscapeChar;		// String Escape Character: \ for \n, \t, \\, etc.
extern const char g_chEscapeChar;

// +++ Token-Typen +++
typedef enum { Empty, Whitespace, SpecialChar, Operator, Keyword, 
               Constant, Identifier, Comment } minTokenType;
// Token-Typen:
//   * Whitespace
//   * spezial char (z.B. ';' '{' '}' )
//	 * Identifier
//   * Operator
//   * Keyword
//   * Constant (z.B. true, false, "einText", 42, 3.1415)
//   * Comment

// +++ Token-Id's +++
const int BOOL_CONST_ID			= 1;
const int STRING_CONST_ID		= 2;
const int INT_NUMBER_ID			= 3;
const int FLOAT_NUMBER_ID		= 4;
const int CHAR_CONST_ID			= 5;

// *** Datentypen ***
const int VOID_ID				= 10;
const int BOOL_ID				= 11;
const int CHAR_ID				= 12;
const int UCHAR_ID				= 13;		// unsigned char
const int INT_ID				= 14;
const int UINT_ID				= 15;		// unsigned, unsigned int
const int SHORT_ID				= 16;		// short, short int
const int USHORT_ID				= 17;		// unsigned short
const int LONG_ID				= 18;		// long, long int
const int ULONG_ID				= 19;		// unsigned long
const int UNSIGNED_ID			= 20;		// wird ersetzt in UINT_ID
const int FLOAT_ID				= 21;
const int DOUBLE_ID				= 22;
const int STRING_ID				= 23;

// *** Kontrollstrukturen ***
const int WHILE_ID				= 30;
const int DO_ID					= 31;
const int FOR_ID				= 32;
const int IF_ID					= 33;
const int ELSE_ID				= 34;
const int RETURN_ID				= 35;
const int BREAK_ID				= 36;
const int CONTINUE_ID			= 37;
const int CLASS_ID				= 38;
const int STRUCT_ID				= 39;
const int THIS_ID				= 40;
const int TYPEDEF_ID			= 41;
const int NEW_ID				= 42;
const int DELETE_ID				= 43;
const int CONST_ID				= 44;
const int VIRTUAL_ID			= 45;
const int PRIVATE_ID			= 46;
const int PROTECTED_ID			= 47;
const int PUBLIC_ID				= 48;
const int EXTERN_ID				= 49;
const int STATIC_ID				= 50;

const int OBJECT_ID				= 60;		// ERWEITERUNG
const int TYPEOF_ID				= 61;		// ERWEITERUNG
const int EXISTS_ID				= 62;		// ERWEITERUNG
const int DEREFEXISTS_ID		= 63;		// ERWEITERUNG
const int DBGHALT_ID			= 64;		// ERWEITERUNG

const int PARENTHIS_OPEN_ID		= 220;
const int PARENTHIS_CLOSE_ID	= 221;
const int STATEMENT_END_ID		= 222;
const int BLOCK_OPEN_ID			= 223;
const int BLOCK_CLOSE_ID		= 224;
const int ARRAY_STOP_ID			= 225;
const int PREPROC_START_ID		= 226;
const int DOT_DOT_ID			= 227;
const int BACKSLASH_ID			= 228;
const int PREPROC_CONCATENATE_ID= 229;

const int ADD_ID				= 100;
const int SUB_ID				= 101;
const int MULT_ID				= 102;
const int DIV_ID				= 103;
const int MOD_ID				= 104;
const int ASSIGN_ID				= 105;
const int LESS_ID				= 106;
const int MORE_ID				= 107;
const int EQUAL_ID				= 108;
const int NOT_EQUAL_ID			= 109;
const int LOG_AND_ID			= 110;
const int LOG_OR_ID				= 111;
const int NOT_ID				= 112;
const int INC_ID				= 113;
const int DEC_ID				= 114;
const int COMMA_ID				= 115;
const int BINARY_AND_ID			= 116;
const int DOT_ID				= 117;
const int ARRAY_START_ID		= 118;
const int INVERT_ID				= 119;
const int PTR_DEREF_ID			= 120;
const int CONDITION_ID			= 121;
const int LESSEQUAL_ID			= 122;
const int MOREEQUAL_ID			= 123;
const int BINARY_OR_ID			= 124;
const int SWITCH_ID				= 125;
const int CASE_ID				= 126;
const int DEFAULT_ID			= 127;
const int GOTO_ID				= 128;
const int LEFTSHIFT_ID			= 129;
const int RIGHTSHIFT_ID			= 130;
const int OPERATOR_ID			= 131;
const int SIZEOF_ID				= 132;
const int TEMPLATE_ID			= 133;

// +++ Datentypen Konstanten +++
#define _VOID				"void"
#define _STRING				"string"
#define _BOOL				"bool"
#define _UNSIGNED			"unsigned"
#define _CHAR				"char"
#define _UCHAR				"unsigned char"
#define _INT				"int"
#define _UINT				"unsigned int"
#define _SHORT				"short"
#define _USHORT				"unsigned short"
#define _LONG				"long"
#define _ULONG				"unsigned long"
#define _FLOAT				"float"
#define _DOUBLE				"double"
#define _FUNCTION			"function"
#define _CONST_MANGLING		"C$"

//*************************************************************************
/** Klasse zur Repraesentation eines Tokens */
class MINDLLEXPORT minToken
{
public:
	minToken()
		: m_aType( Empty ), m_sText( "" ), m_nId( 0 )
	{}
	minToken( minTokenType aType, const string & sText, int nId = 0 )
		: m_aType( aType ), m_sText( sText ), m_nId( nId )
	{}

	// diverse Operatoren
	bool operator==( const minToken & aItem ) const	{ return m_sText == aItem.m_sText; }
	bool operator!=( const minToken & aItem ) const	{ return !((*this)==aItem); }
	bool operator<( const minToken & aItem ) const	{ return m_sText < aItem.m_sText; }
#ifndef __linux__
	bool operator>( const minToken & aItem ) const	{ return m_sText > aItem.m_sText; }
#endif

	bool			IsEmpty() const					{ return m_aType == Empty; }
	bool			IsWhitespace() const			{ return m_aType == Whitespace; }
	bool			IsNewLine() const				{ return IsWhitespace() && (m_sText=="\n" || m_sText=="\r" || m_sText=="\r\n" || m_sText=="\n\r"); }
	bool			IsSpecialChar() const			{ return m_aType == SpecialChar; }
	bool			IsOperator() const				{ return m_aType == Operator; }
	bool			IsKeywordAsOperator() const		{ return IsOperator() && (m_nId==NEW_ID || m_nId==DELETE_ID || m_nId==TYPEOF_ID || m_nId==EXISTS_ID || m_nId==DEREFEXISTS_ID); }
	bool			IsArrayOperator() const			{ return IsOperator() && m_nId==ARRAY_START_ID; }
	bool			IsKeyword() const				{ return m_aType == Keyword; }
	bool			IsConstant() const				{ return m_aType == Constant; }
	bool			IsBool() const					{ return IsConstant() && (m_nId==BOOL_CONST_ID); }
	bool			IsChar() const					{ return IsConstant() && (m_nId==CHAR_CONST_ID); }
	bool			IsIntNumber() const				{ return IsConstant() && (m_nId==INT_NUMBER_ID); }
	bool			IsFloatNumber() const			{ return IsConstant() && (m_nId==FLOAT_NUMBER_ID); }
	bool			IsString() const				{ return IsConstant() && (m_nId==STRING_CONST_ID); }
	bool			IsIdentifier() const			{ return m_aType == Identifier; }
	bool			IsComment() const				{ return m_aType == Comment; }
	// spezial-Abfragen:
	bool			IsStatementTerminator() const	{ return (m_aType==SpecialChar) && (m_nId==STATEMENT_END_ID); }
	bool			IsParenthisOpen() const			{ return (m_aType==SpecialChar) && (m_nId==PARENTHIS_OPEN_ID); }
	bool			IsParenthisClose() const		{ return (m_aType==SpecialChar) && (m_nId==PARENTHIS_CLOSE_ID); }
	bool			IsBlockOpen() const				{ return (m_aType==SpecialChar) && (m_nId==BLOCK_OPEN_ID); }
	bool			IsBlockClose() const			{ return (m_aType==SpecialChar) && (m_nId==BLOCK_CLOSE_ID); }
	bool			IsArrayStop() const				{ return (m_aType==SpecialChar) && (m_nId==ARRAY_STOP_ID); }
	bool			IsPreProcessorStart() const		{ return (m_aType==SpecialChar) && (m_nId==PREPROC_START_ID); }
	bool			IsPreProcConcatenate() const	{ return (m_aType==SpecialChar) && (m_nId==PREPROC_CONCATENATE_ID); }
	bool			IsBackslash() const				{ return (m_aType==SpecialChar) && (m_nId==BACKSLASH_ID); }
	bool			IsHexConstant() const			{ return IsIntNumber() && (m_sText.substr( 0, strlen( g_sHexStart ) )==string( g_sHexStart )); }

	minTokenType	GetType() const					{ return m_aType; }
	int				GetId() const					{ return m_nId; }
	const string &	GetString() const				{ return m_sText; }
	string			GetRepresentationString() const	
	{ 
		string s; 
		if( IsString() ) 
		{
			s = "\"";
		}
		else if( IsChar() )
		{
			s = "'";
		}
		return s+m_sText+s; 
	}

private:
	minTokenType	m_aType;
	int				m_nId;
	string			m_sText;
};

#endif

