/**************************************************************************
 *
 *	project				 : minscript
 *
 *	copyright            : (C) 1999-2003 by Michael Neuroth
 *
 * ------------------------------------------------------------------------
 *
 *  $Source: /Users/min/Documents/home/cvsroot/minscript/minscript.cpp,v $
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

#define _USE_DLL
#include "dllexport.h"

#include "platform.h"

#include "minparsernodes.h"
#include "minip.h"

#include "mstl_string.h"
#include "mstl_iostream.h"

#include <stdio.h>			// fuer: sprintf()

#define _MINSCRIPT_VERSION	"1.2.1"

#define _REGISTER_FCN_NAME	"minRegisterNativeFunctions"

//*************************************************************************
//*************************************************************************
//*************************************************************************

typedef list<string> StringContainerT;

struct minArgumentsHelper
{
	minArgumentsHelper()
	{
		m_bShowVersion = false;
		m_bShowHelp = false;
		m_bShowDebug = false;
		//m_bVerbose = false;
		m_bShowLicense = false;
		m_bDump = false;
		m_bProfile = false;
		m_bDebugModus = false;
		m_bRunScript = true;			// default: immer script ausfuehren
		m_bParseOnly = !m_bRunScript;
		m_bRunPreprocessor = true;		// default: immer script mit preprocessor verarbeiten (vor script ausfuehrung)
		m_bRunCodegen = false;
		m_bMakeStubs = false;
		m_bMakeWrapper = false;
		m_bMakeDll = false;
	}

	StringContainerT	m_aFileNameList;
	//StringContainerT	m_aDllNameList;
	StringContainerT	m_aDefineList;
	StringContainerT	m_aIncludePathList;
	StringContainerT	m_aArgumentList;
	string				m_sScript;
	string				m_sOutputFileName;

	bool				m_bShowVersion;
	bool				m_bShowHelp;

	bool				m_bShowDebug;
	//bool				m_bVerbose;
	bool				m_bShowLicense;
	bool				m_bDump;
	bool				m_bProfile;
	bool				m_bDebugModus;

	bool				m_bRunScript;
	bool				m_bParseOnly;
	bool				m_bRunPreprocessor;
	bool				m_bRunCodegen;
	bool				m_bMakeStubs;
	bool				m_bMakeWrapper;
	bool				m_bMakeDll;
};

static bool minParseArgs( int argc, char * argv[], minArgumentsHelper & aArgs )
{
	int i=1;
	while( i<argc )
	{
		string sActArg( argv[i] );

		if( sActArg=="-v" || sActArg=="--ver" || sActArg=="--version" )
		{
			aArgs.m_bShowVersion = true;
		}
		else if( sActArg=="-?" || sActArg=="-h" || sActArg=="--help" )
		{
			aArgs.m_bShowHelp = true;
		}
		else if( sActArg=="-c" || sActArg=="--check" || sActArg=="--syntax-check" || sActArg=="--parse" )
		{
			aArgs.m_bParseOnly = true;
			aArgs.m_bRunScript = false;
		}
		else if( sActArg=="--nopreproc" )
		{
			aArgs.m_bRunPreprocessor = false;
		}
		else if( sActArg=="--onlypreproc" )
		{
			aArgs.m_bRunPreprocessor = true;
			aArgs.m_bRunScript = false;
		}
#ifdef USEBIG
		else if( sActArg=="--codegen" )
		{
			aArgs.m_bRunCodegen = true;
			aArgs.m_bRunScript = false;
		}
		else if( sActArg=="--makestubs" )
		{
			aArgs.m_bMakeStubs = true;
			aArgs.m_bRunScript = false;
		}
		else if( sActArg=="--makewrappers" )
		{
			aArgs.m_bMakeWrapper = true;
			aArgs.m_bRunScript = false;
		}
		else if( sActArg=="--makedll" )
		{
			aArgs.m_bMakeDll = true;
			aArgs.m_bRunScript = false;
		}
#endif
		else if( sActArg=="--debug" )
		{
			aArgs.m_bShowDebug = true;
		}
		else if( sActArg=="--dump" )
		{
			aArgs.m_bDump = true;
		}
		else if( sActArg=="--license" )
		{
			aArgs.m_bShowLicense = true;
		}
		/*else if( sActArg=="--verbose" )
		{
			aArgs.m_bVerbose = true;
		}*/
		else if( sActArg=="--profile" )
		{
			aArgs.m_bProfile = true;
		}
		else if( sActArg=="-g" )
		{
			aArgs.m_bDebugModus = true;
		}
		else if( sActArg=="-s" || sActArg=="--script" )
		{
			i++;
			if( i<argc )
				aArgs.m_sScript = argv[i];
			else
				return false;
		}
		else if( sActArg=="-a" || sActArg=="--arg" )
		{
			i++;
			if( i<argc )
				aArgs.m_aArgumentList.push_back( argv[i] );
			else
				return false;
		}
		else if( (sActArg=="-d") || (sActArg=="-D") )
		{
			// jetzt noch das zu definierende Symbol lesen
			i++;
			if( i<argc )
				aArgs.m_aDefineList.push_back( argv[i] );
			else
				return false;
		}
		else if( (sActArg=="-i") || (sActArg=="-I") )
		{
			// jetzt noch das zusaetzliche Include-Verzeichnis lesen
			i++;
			if( i<argc )
				aArgs.m_aIncludePathList.push_back( argv[i] );
			else
				return false;
		}
/*		else if( sActArg=="--dll" )
		{
			// jetzt noch den Namen der DLL lesen
			i++;
			if( i<argc )
				aArgs.m_aDllNameList.push_back( argv[i] );
			else
				// kein Argument als Dll-Name angegeben
				return false;
		}
*/		else if( sActArg=="-o" || sActArg=="--output" )
		{
			// jetzt noch den Namen der Ausgabedatei lesen
			i++;
			if( i<argc )
				aArgs.m_sOutputFileName = argv[i];
			else
				return false;
		}
		else if( sActArg.length()>0 && sActArg.at( 0 )=='-' )
		{
			// unbekanntes Argument gefunden...
			return false;
		}
		else
		{
			// dann muss es ein Dateiname sein
			aArgs.m_aFileNameList.push_back( sActArg );
		}

		i++;
	}
	return true;
}

static void ShowArgumentHelp( ostream & aStream )
{
	aStream << endl;
	aStream << "Help for minscript:" << endl << endl;
	aStream << "Options:" << endl;
	aStream << "  -?, -h, --help                  : show help for the program" << endl;
	aStream << "  -v, --ver, --version            : show program version" << endl;
	aStream << "  -d definesymbol                 : define a symbol for preprocessor" << endl;
	aStream << "  -i directory                    : sets an aditional path for include files" << endl;
	aStream << "  -g                              : enable debug execution" << endl;
	aStream << "  -s \"script\", --script \"script\"  : script to execute" << endl;
	aStream << "  -o output, --output output      : write output to file with name output" << endl;
	aStream << "  -c, --check, --parse            : do only syntax check for script (parsing)" << endl;
	aStream << "  -a argument, --arg argument     : argument for script execution" << endl;
	//aStream << "  --dll dllname                   : load dll-module with name dllname" << endl;
	SMALL( aStream << "  --makestubs                     : generate stubfile for headerfile" << endl; )
	SMALL( aStream << "  --makewrappers                  : generate wrapper classes for headerfile" << endl; )
	SMALL( aStream << "  --makedll                       : show compiler options to compile stubfiles" << endl; )
	SMALL( aStream << "  --codegen                       : generate c++ code for script" << endl; )
	aStream << "  --nopreproc                     : do not run preprocessor" << endl;
	aStream << "  --onlypreproc                   : run only preprocessor" << endl;
	aStream << "  --profile                       : profile script execution" << endl;
	aStream << "  --dump                          : show processing information" << endl;
	//aStream << "  --verbose                       : show more processing information" << endl;
	SMALL( aStream << "  --debug                         : show debugging output" << endl; )
	aStream << "  --license                       : show license information" << endl;
	aStream << "  scriptfilename                  : script file to execute" << endl;
}

static void ShowLicense( ostream & aStream )
{
	aStream << endl << "License for minscript: free for non-commercial use." << endl;
	aStream << "Contact the author if you want to use minscript commercially." << endl << endl;
	aStream << "Homepage: http://people.freenet.de/mneuroth/zaurus/minscript.html" << endl;
	aStream << "Author:   michael.neuroth@freenet.de" << endl << endl;
}

//*************************************************************************

// Hilfsfunktion: int --> string
static string minLTOA( int n )
{
	char sBuffer[c_iMaxBuffer];
	sprintf( sBuffer, "%d", n );
	return sBuffer;
}

#ifdef USEBIG

const char * c_sThisType		= "int";	// "void *";
const char * c_sDestructorName	= "__Destructor_";
const char * c_sWarning1 = "// WARNING: This file is automatically generated by minscript with option "; 
const char * c_sWarning2 = "//          This file will be overwritten, do not change it manually!\n";

static void ShowCompileOptionsForDll( ostream & aStream )
{
#if defined( _MSC_VER )
#ifdef _DEBUG
	aStream << "cl -c -Zi -GX -GR -LD -MDd " << endl;
#else
	aStream << "cl -c -O2 -GX -GR -LD -MD " << endl;
#endif
#elif defined( __linux__ )
	aStream << "gcc -c -fexceptions -fPIC -shared " << endl;
#else
	aStream << "Error: unknown compiler configuration" << endl;
#endif
}

static string GetCallArguments( string & sArgumentsForCall, const minVariableDeclarationList & aArgsList )
{
	string sFcnProtoType;

	minVariableDeclarationList::const_iterator aVarIter = aArgsList.begin();
	int iCount = 0;
	while( aVarIter != aArgsList.end() )
	{
		char sArgName[512];

		minVariableDeclarationList::value_type aItem = *aVarIter;

		sFcnProtoType += aItem->GetType().GetTypeString();
		sFcnProtoType += " ";
		sprintf( sArgName, "arg%d", iCount );
		sFcnProtoType += sArgName;

		++iCount;
		++aVarIter;

		sArgumentsForCall += sArgName;
		if( aVarIter != aArgsList.end() )
		{
			sArgumentsForCall += ", ";
			sFcnProtoType += ", ";
		}
	}

	return sFcnProtoType;
}

static minHandle<minInterpreterNode> CopyFunctionAndAddThisArg( minHandle<minInterpreterNode> hFcn ) 
{
	// schmuggele c_sThisType (int) in die Argument-Liste
	// dazu eine neue Funktion anlegen, als Kopie der aktuellen Funktion
	// und deren Argument-Liste manipulieren. Fuer diese neue Funktion
	// die Funktion-Registrierung durchfuehren
	minFunctionDeclarationNode * pFcn = (minFunctionDeclarationNode *)(hFcn.GetPtr());
	minFunctionDeclarationNode * pNewFcn = new minFunctionDeclarationNode( *pFcn );
	minVariableDeclarationList aNewArgs = pNewFcn->GetArgumentsList();
	minHandle<minVariableDeclarationNode> hThisArg( new minVariableDeclarationNode( "pThis", minInterpreterType( Int /*Void, 1*/ ), -1, 0, 0, 0, StringListT() ) );
	aNewArgs.insert( aNewArgs.begin(), hThisArg );
	pNewFcn->SetArgumentsList( aNewArgs );
	return minHandle<minInterpreterNode>( pNewFcn );
}

static minHandle<minInterpreterNode> CopyFunctionAndSetReturnType( minHandle<minInterpreterNode> hFcn ) 
{
	minFunctionDeclarationNode * pFcn = (minFunctionDeclarationNode *)(hFcn.GetPtr());
	minFunctionDeclarationNode * pNewFcn = new minFunctionDeclarationNode( *pFcn );
	pNewFcn->SetReturnType( minInterpreterType( Int /*Void, 1*/ ) );
	return minHandle<minInterpreterNode>( pNewFcn );
}

static minHandle<minInterpreterNode> CopyFunctionAndSetVoidReturnType( minHandle<minInterpreterNode> hFcn ) 
{
	minFunctionDeclarationNode * pFcn = (minFunctionDeclarationNode *)(hFcn.GetPtr());
	minFunctionDeclarationNode * pNewFcn = new minFunctionDeclarationNode( *pFcn );
	pNewFcn->SetReturnType( minInterpreterType( Void, 0 ) );
	return minHandle<minInterpreterNode>( pNewFcn );
}

static bool SplitFileName( const string & sFileNameWithExt, string & sFileName )
{
	int iPos = sFileNameWithExt.find( "." );
	if( iPos != string::npos )
	{
		sFileName = sFileNameWithExt.substr( 0, iPos );
	}
	else
	{
		sFileName = sFileNameWithExt;
	}
	return true;
}

static bool MakeClassWrapper( const string & sDllName, string & sWrapperCode, const minParserItemList & aClassItemList )
{
	string sTempDllName;
	SplitFileName( sDllName, sTempDllName );

	sWrapperCode += "// load the needed module\n";
	sWrapperCode += "__loaddll( \"";
	sWrapperCode += sTempDllName;
	sWrapperCode += "\" );\n\n";

	minParserItemList::const_iterator aClassIter = aClassItemList.begin();
	while( aClassIter != aClassItemList.end() )
	{
		minHandle<minInterpreterNode> hClassProto = *aClassIter;

		if( hClassProto.IsOk() )
		{
			InterpreterClassScope		aClassScope = None;
			minClassDeclarationNode *	pClassProto = (minClassDeclarationNode *)hClassProto.GetPtr();
			minClassBlockNode *			pClassBlock = (minClassBlockNode *)pClassProto->GetClassBlock();

			sWrapperCode += "class ";
			sWrapperCode += pClassProto->GetName();
			sWrapperCode += "\n{\n";
			sWrapperCode += "public:\n";
			aClassScope = Public;

			// Konstruktor anlengen
			const minParserItemList	& aConstructorList = pClassBlock->GetConstructors();
			minParserItemList::const_iterator aConstrIter = aConstructorList.begin();
			while( aConstrIter != aConstructorList.end() )
			{
				minFunctionDeclarationNode * pFcn = (minFunctionDeclarationNode *)(*aConstrIter).GetPtr();

				// zur Zeit nur Public Methoden exportieren !
				if( pFcn->GetClassScope() == Public )
				{
					string sFcnProtoType;
					string sArgumentsForCall;

					string sCallArgs = GetCallArguments( sArgumentsForCall, pFcn->GetArgumentsList() );
					sFcnProtoType += sCallArgs;

					sWrapperCode += "\t";
					sWrapperCode += pClassProto->GetName();
					sWrapperCode += "( ";
					sWrapperCode += sFcnProtoType;
					sWrapperCode += " )\n";
					sWrapperCode += "\t{\n";
					sWrapperCode += "\t\tm_pCppThis = ";
					sWrapperCode += pClassProto->GetName();
					sWrapperCode += "_";
					sWrapperCode += pClassProto->GetName();
					sWrapperCode += "( ";
					sWrapperCode += sArgumentsForCall;
					sWrapperCode += " );\n";
					sWrapperCode += "\t}\n";
				}

				++aConstrIter;
			}

			// Destruktor anlengen
			sWrapperCode += "\t~";
			sWrapperCode += pClassProto->GetName();
			sWrapperCode += "()\n";
			sWrapperCode += "\t{\n";
			sWrapperCode += "\t\t";
			sWrapperCode += c_sDestructorName;
			sWrapperCode += pClassProto->GetName();
			sWrapperCode += "( m_pCppThis );\n";
			sWrapperCode += "\t}\n";

			sWrapperCode += "\n";

			string sMethodSeparator( GetMethodSeparatorStrg() );

			minParserItemList aList = pClassBlock->GetMethods();
			minParserItemList::const_iterator aIter = aList.begin();
			while( aIter != aList.end() )
			{
				minFunctionDeclarationNode * pFcn = (minFunctionDeclarationNode *)(*aIter).GetPtr();

				if( pFcn )
				{
					InterpreterClassScope aNewClassScope = pFcn->GetClassScope();

					if( aNewClassScope != aClassScope )
					{
						switch( aNewClassScope )
						{
							case Public:
								sWrapperCode += "public:\n";
								break;
							case Protected:
								sWrapperCode += "protected:\n";
								break;
							case Private:
								sWrapperCode += "private:\n";
								break;
						}
						aClassScope = aNewClassScope;
					}

					// zur Zeit nur Public Methoden exportieren !
					if( aClassScope == Public )
					{
						string sArgumentsForCall;
						string sFcnProtoType;

						string sTempName = pFcn->GetName(); 
						int iFindPos = sTempName.find( sMethodSeparator );
						sTempName.replace( iFindPos, sMethodSeparator.length(), "_" );

						string sMethodName = pFcn->GetName(); 
						sMethodName = sMethodName.substr( iFindPos+1 );

						bool bIsVoid = (pFcn->GetReturnType() == Void);

						// Funktions-Kopf erzeugen: int MyClass_f( int i )
						if( pFcn->IsVirtual() )
						{
							sFcnProtoType += "virtual ";
						}
						sFcnProtoType += pFcn->GetReturnType().GetTypeString();
						sFcnProtoType += " ";
						sFcnProtoType += sMethodName;
						sFcnProtoType += "( ";

						string sCallArgs = GetCallArguments( sArgumentsForCall, pFcn->GetArgumentsList() );
						sFcnProtoType += sCallArgs;

						sFcnProtoType += " )";

						if( pFcn->IsConstant() )
						{
							sFcnProtoType += " const";
						}

						sWrapperCode += "\t";
						sWrapperCode += sFcnProtoType;

						// Funktions-Rumpf erzeugen
						sWrapperCode += "\n\t{\n";
						sWrapperCode += "\t\t";
						if( !bIsVoid )
						{
							sWrapperCode += "return ";
						}
						sWrapperCode += sTempName;
						sWrapperCode += "( m_pCppThis";
						if( sArgumentsForCall.length()>0 )
						{
							sWrapperCode += ", ";
						}
						sWrapperCode += sArgumentsForCall;
						sWrapperCode += " );\n";

						sWrapperCode += "\t}\n";
					}
				}

				++aIter;
			}
			
			sWrapperCode += "private:\n";
			sWrapperCode += "\t";
			sWrapperCode += c_sThisType;
			sWrapperCode += "\tm_pCppThis;\n";
			sWrapperCode += "};\n";
		}

		++aClassIter;
	}
	return true;
}

static bool MakeFunctionRegistration( string & sStubCode, minHandle<minInterpreterNode> hFcnProto, const string & sRealMethodName = "", const string & sRealProtoType = "" )
{
	const minFunctionDeclarationNode * pFcnProto = (const minFunctionDeclarationNode *)hFcnProto.GetPtr();
	const minVariableDeclarationList & aArgList	= pFcnProto->GetArgumentsList();
	bool bIsVoidFcn = pFcnProto->GetReturnType()==Void;

	sStubCode += "\t";
	if( bIsVoidFcn )
		sStubCode += "pFcn = new NativeVoidFcnWrapper";
	else
		sStubCode += "pFcn = new NativeFcnWrapper";
	sStubCode += minLTOA( aArgList.size() );

	if( !(bIsVoidFcn && aArgList.size()==0) )
	{
		sStubCode += "<";
	}

	if( !bIsVoidFcn )
	{
		sStubCode += pFcnProto->GetReturnType().GetTypeString();
		if( aArgList.size()>0 )
		{
			sStubCode += ",";
		}
	}

	minVariableDeclarationList::const_iterator aArgIter = aArgList.begin();
	while( aArgIter != aArgList.end() )
	{
		minHandle<minVariableDeclarationNode> hVarDecl = *aArgIter;

		sStubCode += hVarDecl->GetType().GetTypeString();

		++aArgIter;

		if( aArgIter != aArgList.end() )
		{
			sStubCode += ",";
		}
	}

	if( !(bIsVoidFcn && aArgList.size()==0) )
	{
		sStubCode += ">";
	}
	
	sStubCode += "( ";
	if( sRealMethodName.length()>0 )
	{
		sStubCode += sRealMethodName;
	}
	else
	{
		sStubCode += pFcnProto->GetName();
	}
	sStubCode += ", \"";
	if( sRealProtoType.length()>0 )
	{
		sStubCode += sRealProtoType;
	}
	else
	{
		sStubCode += pFcnProto->GetPrototypeString();
	}
	sStubCode += "\" );";
	sStubCode += "\n";

	sStubCode += "\taEnvironment.AddFunction( minHandle<minFunctionDeclarationNode>( new minNativeFunctionDeclarationNode( pFcn ) ) );\n";

	return true;
}

static bool MakeFunctionStubs( string & sStubCode, const minParserItemList & aItemList, string & sAdditionaRegistrations )
{
	sStubCode += "extern \"C\" bool CPPDLLEXPORT ";
	sStubCode += _REGISTER_FCN_NAME;
	sStubCode += "( minInterpreterEnvironment & aEnvironment )\n";
	sStubCode += "{\n";
	sStubCode += "\tminNativeFcnInterface * pFcn;\n";
	sStubCode += "\n";

	minParserItemList::const_iterator aIter = aItemList.begin();
	while( aIter != aItemList.end() )
	{
		minHandle<minInterpreterNode> hFcnProto = *aIter;

		MakeFunctionRegistration( sStubCode, hFcnProto );

		++aIter;
	}

	sStubCode += "\n\t// additional registrations\n";
	sStubCode += sAdditionaRegistrations;

	sStubCode += "\n";
	sStubCode += "\treturn true;\n";
	sStubCode += "}\n";

	return true;
}

static bool MakeClassStubs( string & sStubCode, const minParserItemList & aClassItemList, string & sMethodRegistrations )
{
	minParserItemList::const_iterator aClassIter = aClassItemList.begin();
	while( aClassIter != aClassItemList.end() )
	{
		minHandle<minInterpreterNode> hClassProto = *aClassIter;

		if( hClassProto.IsOk() )
		{
			sStubCode += "\n";

			string sMethodSeparator( GetMethodSeparatorStrg() );

			minClassDeclarationNode *	pClassProto = (minClassDeclarationNode *)hClassProto.GetPtr();
			minClassBlockNode *			pClassBlock = (minClassBlockNode *)pClassProto->GetClassBlock();

			// Konstruktoren Stubs anlegen, damit Objekt ueberhaupt angelegt werden kann
			const minParserItemList	& aConstructorList = pClassBlock->GetConstructors();
			minParserItemList::const_iterator aConstrIter = aConstructorList.begin();
			while( aConstrIter != aConstructorList.end() )
			{
				minFunctionDeclarationNode * pFcn = (minFunctionDeclarationNode *)(*aConstrIter).GetPtr();

				// zur Zeit nur Public Methoden exportieren !
				if( pFcn->GetClassScope() == Public )
				{
					string sFcnProtoType;
					string sArgumentsForCall;

					sFcnProtoType += c_sThisType;
					sFcnProtoType += " ";
					sFcnProtoType += pClassProto->GetName();
					sFcnProtoType += "_";
					sFcnProtoType += pClassProto->GetName();

					sFcnProtoType += "( ";

					string sCallArgs = GetCallArguments( sArgumentsForCall, pFcn->GetArgumentsList() );
					sFcnProtoType += sCallArgs;

					sFcnProtoType += " )";

					sStubCode += sFcnProtoType;
					sFcnProtoType += ";";
					sStubCode += "\n{\n";
					sStubCode += "\treturn (";
					sStubCode += c_sThisType;
					sStubCode += ")new ";
					sStubCode += pClassProto->GetName();
					sStubCode += "( ";
					sStubCode += sArgumentsForCall;
					sStubCode += " );\n";
					sStubCode += "}\n\n";

					// registriere Contruktor-Funktion
					minHandle<minInterpreterNode> hNewConstructor = CopyFunctionAndSetReturnType( *aConstrIter );
					MakeFunctionRegistration( sMethodRegistrations, hNewConstructor, pClassProto->GetName()+"_"+pClassProto->GetName(), sFcnProtoType );
				}
				++aConstrIter;
			}

			// Destruktor Stub anlegen, damit Objekt wieder zerstoert werden kann
			string sDestructorTemp;
			sDestructorTemp += c_sDestructorName;
			sDestructorTemp += pClassProto->GetName();

			string sDestructorProtoTemp( "void " );
			sDestructorProtoTemp += sDestructorTemp;
			sDestructorProtoTemp += "( ";
			sDestructorProtoTemp += c_sThisType;
			sDestructorProtoTemp += " pThis )";
			sStubCode += sDestructorProtoTemp;
			sDestructorProtoTemp += ";";
			sStubCode += "\n{\n";
			sStubCode += "\t";
			sStubCode += pClassProto->GetName();
			sStubCode += " * pClass = (";
			sStubCode += pClassProto->GetName();
			sStubCode += "*)pThis;\n\n";
			sStubCode += "\tif( pClass )\n";
			sStubCode += "\t{\n";
			sStubCode += "\t\tdelete pClass;\n";
			sStubCode += "\t}\n";
			sStubCode += "}\n\n";
			// registriere Destruktor-Funktion
			const minHandle<minInterpreterNode>	hDestructor = pClassBlock->GetDestructor();
			minHandle<minInterpreterNode> hNewDestructorTemp = CopyFunctionAndAddThisArg( hDestructor );
			minHandle<minInterpreterNode> hNewDestructor = CopyFunctionAndSetVoidReturnType( hNewDestructorTemp ) ;
			MakeFunctionRegistration( sMethodRegistrations, hNewDestructor, sDestructorTemp, sDestructorProtoTemp );

			minParserItemList aList = pClassBlock->GetMethods();
			minParserItemList::const_iterator aIter = aList.begin();
			while( aIter != aList.end() )
			{
				minFunctionDeclarationNode * pFcn = (minFunctionDeclarationNode *)(*aIter).GetPtr();

				if( pFcn )
				{
					string sArgumentsForCall;
					string sFcnProtoType;

					// zur Zeit nur Public Methoden exportieren !
					if( pFcn->GetClassScope() == Public )
					{
						string sTempName = pFcn->GetName(); 
						int iFindPos = sTempName.find( sMethodSeparator );
						sTempName.replace( iFindPos, sMethodSeparator.length(), "_" );

						string sMethodName = pFcn->GetName(); 
						sMethodName = sMethodName.substr( iFindPos+1 );

						bool bIsVoid = (pFcn->GetReturnType() == Void);

						// Funktions-Kopf erzeugen: int MyClass_f( int i )
						sFcnProtoType += pFcn->GetReturnType().GetTypeString();
						sFcnProtoType += " ";
						sFcnProtoType += sTempName;
						sFcnProtoType += "( ";

						// this Argument erzeugen
						sFcnProtoType += c_sThisType;
						sFcnProtoType += " pThis";

						string sCallArgs = GetCallArguments( sArgumentsForCall, pFcn->GetArgumentsList() );
						if( sCallArgs.length()>0 )
						{
							sFcnProtoType += ", ";
						}
						sFcnProtoType += sCallArgs;

						sFcnProtoType += " )";

						sStubCode += sFcnProtoType;

						sFcnProtoType += ";";
						minHandle<minInterpreterNode> hNewFcn = CopyFunctionAndAddThisArg( *aIter );
 						MakeFunctionRegistration( sMethodRegistrations, hNewFcn, sTempName, sFcnProtoType );

						// Funktions-Rumpf erzeugen
						sStubCode += "\n{\n";
						sStubCode += "\t";
						sStubCode += pClassProto->GetName();
						sStubCode += " * pClass = (";
						sStubCode += pClassProto->GetName();
						sStubCode += "*)pThis;\n\n";
						sStubCode += "\tif( pClass )\n";
						sStubCode += "\t{\n";
						sStubCode += "\t\t";
						if( !bIsVoid )
						{
							sStubCode += "return ";
						}
						sStubCode += "pClass->";
						sStubCode += sMethodName;
						sStubCode += "( ";
						sStubCode += sArgumentsForCall;
						sStubCode += " );\n";
						sStubCode += "\t}\n";
						if( !bIsVoid )
						{
							sStubCode += "\treturn 0;\n";
						}
						sStubCode += "}\n";
					}
					sStubCode += "\n";
				}

				++aIter;
			}
		}

		++aClassIter;
	}

	return true;
}

static bool MakeStubFiles( const minArgumentsHelper & aArgs, const minParserItemList & aItemList, const minParserItemList & aClassItemList )
{
	string sStubCode;

	sStubCode += "\n";
	sStubCode += c_sWarning1;
	sStubCode += "\"--makestubs\".\n";
	sStubCode += c_sWarning2;
	sStubCode += "\n";
	sStubCode += "#define _USE_DLL\n";
	sStubCode += "#include \"platform.h\"\n";
	sStubCode += "#include \"dllexport.h\"\n";
	sStubCode += "#include \"minhandle.h\"\n";
	sStubCode += "#include \"minscript_cpp.h\"\n";
	sStubCode += "#include \"minnativehlp.h\"\n";
	sStubCode += "#include \"minip.h\"\n";
	//sStubCode += "#undef _USE_DLL\n";
	sStubCode += "\n";

	StringContainerT::const_iterator aNameIter = aArgs.m_aFileNameList.begin();
	while( aNameIter != aArgs.m_aFileNameList.end() )
	{
		sStubCode += "#include \"";
		sStubCode += *aNameIter;
		sStubCode += "\"\n";

		++aNameIter;
	}

	sStubCode += "\n";

	string sMethodRegistrations;
	MakeClassStubs( sStubCode, aClassItemList, sMethodRegistrations );

	MakeFunctionStubs( sStubCode, aItemList, sMethodRegistrations );	
	
	if( aArgs.m_sOutputFileName.length()>0 )
	{
		if( !WriteAsciiFile( aArgs.m_sOutputFileName.c_str(), sStubCode ) )
		{
			cerr << "error writing output file " << aArgs.m_sOutputFileName.c_str() << "." << endl;
		}
	}
	else
	{
		cout << sStubCode.c_str() << endl;
	}

	return true;
}

static bool MakeWrapperFiles( const minArgumentsHelper & aArgs, const minParserItemList & aClassItemList )
{
	string sWrapperCode;

	sWrapperCode += "\n";
	sWrapperCode += c_sWarning1;
	sWrapperCode += "\"--makewrappers\".\n";
	sWrapperCode += c_sWarning2;
	sWrapperCode += "\n";

	MakeClassWrapper( aArgs.m_sOutputFileName, sWrapperCode, aClassItemList );

	if( aArgs.m_sOutputFileName.length()>0 )
	{
		if( !WriteAsciiFile( aArgs.m_sOutputFileName.c_str(), sWrapperCode ) )
		{
			cerr << "error writing output file " << aArgs.m_sOutputFileName.c_str() << "." << endl;
		}
	}
	else
	{
		cout << sWrapperCode.c_str() << endl;
	}

	return true;
}

#endif

void RunPreproc( bool bOnlyPreproc, const minArgumentsHelper & aArgs, minScriptInterpreter & aIp, string & sScript, minTokenizer::TokenContainerT & aParsedTokens )
{
	// als Parameter vordefinierte Symbole vorbereiten
	string sPredefinedSymbols;

	sPredefinedSymbols += "#define __MINSCRIPT__ 1\n";
	sPredefinedSymbols += "#define __STDC__ 1\n";
	sPredefinedSymbols += "#define __cplusplus 1\n";
#if defined( _WIN32 )
	sPredefinedSymbols += "#define _WIN32 1\n";
#elif defined( __OS2__ )
	sPredefinedSymbols += "#define __OS2__ 1\n";
#elif defined( __linux__ )
	sPredefinedSymbols += "#define __linux__ 1\n";
#else
	sPredefinedSymbols += "#define __UNKNOWN__ 1\n";
#endif
	if( aArgs.m_bDebugModus )
	{
		sPredefinedSymbols += "#define _DEBUG 1\n";
	}
	else
	{
		sPredefinedSymbols += "#define _NDEBUG 1\n";
	}
	if( aArgs.m_aDefineList.size()>0 )
	{
		StringContainerT::const_iterator aIter = aArgs.m_aDefineList.begin();
		while( aIter != aArgs.m_aDefineList.end() )
		{
			sPredefinedSymbols += "#define ";
			sPredefinedSymbols += *aIter;
			sPredefinedSymbols += "\n";
			++aIter;
		}
	}

	if( aArgs.m_bRunPreprocessor )
	{
		string sPreProcessedScript;
		unsigned long nPreprocessingStartTime = minGetCurrentTickCount();
		bool bOk = aIp.RunPreProcessor( bOnlyPreproc, sPredefinedSymbols+sScript, sPreProcessedScript, aArgs.m_aIncludePathList, aParsedTokens );
		if( !bOk )
		{
			cerr << "error in preprocessor" << endl;
			exit( -4 );
		}
		// nach dem Preprocessor Run ist dieses das auszufuehrende Script
		sScript = sPreProcessedScript;	
		unsigned long nPreprocessingStopTime = minGetCurrentTickCount();

		// wo soll die Ausgabe hin?
		if( !aArgs.m_bRunScript )
		{
			// ist ein Ausgabe-Dateiname angegeben, diesen verwenden
			if( aArgs.m_sOutputFileName.length()>0 )
			{
				if( !WriteAsciiFile( aArgs.m_sOutputFileName.c_str(), sScript ) )
				{
					cerr << "error writing output file " << aArgs.m_sOutputFileName.c_str() << "." << endl;
				}
			}
			// ansonsten falls nur Preprocessing erwuenscht auf die Standardausgabe ausgeben
			else if( !aArgs.m_bParseOnly && !aArgs.m_bRunCodegen && !aArgs.m_bMakeStubs && !aArgs.m_bMakeWrapper && !aArgs.m_bMakeDll )
			{
				cout << sScript.c_str() << endl;
			}
		}

		if( aArgs.m_bProfile )
		{
			cout << "time for preprocessing [ms]: " << (1000*(nPreprocessingStopTime - nPreprocessingStartTime))/minGetTickCountPerSec() << endl;
		}
	}
}

//*************************************************************************
int main( int argc, char *argv[] )
{
	int nRet = 0;
	
	{	// Block zum Speicherleck Testen

	// Interpreter anlegen
	minScriptInterpreter aIp;

	// zum merken der vom Preprocessor geparsten Tokens
	minTokenizer::TokenContainerT aParsedTokens;

	// jetzt die Argumente parsen und interpretieren:
	minArgumentsHelper aArgs;
	if( !minParseArgs( argc, argv, aArgs ) )
	{
		cerr << "wrong argument found." << endl;
		ShowArgumentHelp( cerr );
		return -1;
	}
	if( aArgs.m_bShowHelp )
	{
		ShowArgumentHelp( cout );
		return 0;
	}
	if( aArgs.m_bShowLicense )
	{
		ShowLicense( cout );
		return 0;
	}
	if( aArgs.m_bShowVersion )
	{
		cout << "minscript, version " << _MINSCRIPT_VERSION << " from " << __DATE__ /*<< endl*/;
		cout << ", (c) by Michael Neuroth, 1999-2003" << endl;
		return 0;
	}
#ifdef USEBIG
	if( aArgs.m_bMakeDll )
	{
		ShowCompileOptionsForDll( cout );
		return 0;
	}
#endif
/*
	if( aArgs.m_aDllNameList.size()>0 )
	{
		StringContainerT::const_iterator aIter = aArgs.m_aDllNameList.begin();
		while( aIter != aArgs.m_aDllNameList.end() )
		{
			if( !aIp.LoadModule( (*aIter).c_str(), 0 ) )
			{
				// wird ein benoetigtes Modul nicht gefunden, Ausfuehrung sofort abbrechen
				cerr << "error loading minscript-dll: " << (*aIter).c_str() << endl;
				return -2;
			}
			++aIter;
		}
	}
*/
	// jetzt das Script ausfuehren: --script geht vor Script-File
	string sScript;
	if( aArgs.m_sScript.length()>0 )
	{
		sScript = aArgs.m_sScript;
	}
	else if( aArgs.m_aFileNameList.size()>0 )
	{
		StringContainerT::const_iterator aIter = aArgs.m_aFileNameList.begin();
		while( aIter != aArgs.m_aFileNameList.end() )
		{
			string sScriptTemp;
			if( aArgs.m_bShowDebug )
			{
				cout << "reading: " << (*aIter).c_str() << endl;
			}
			if( ReadScript( (*aIter).c_str(), sScriptTemp ) )
			{
				sScript += sScriptTemp;
			}
			else
			{
				cerr << "script file " << (*aIter).c_str() << " not found." << endl;
			}
			++aIter;
		}
	}
	else
	{
		cerr << "error: no script files." << endl;
		ShowArgumentHelp( cerr );
		return -3;
	}

	if( aArgs.m_bShowDebug )
	{
		cout << "Executing script:" << endl << sScript.c_str() << endl;
	}

	// Debug-Modus setzten
	SetDebugModus( aArgs.m_bShowDebug );
	aIp.SetDebugModus( aArgs.m_bShowDebug );

	// nur wenn das Skript nicht ausgefuehrt werden soll an dieser Stelle
	// (OHNE den Argumenten-Code) den Preprocessor aufrufen
	if( aArgs.m_bRunPreprocessor && !aArgs.m_bRunScript )
	{
		RunPreproc( /*bOnlyPreproc*/true, aArgs, aIp, sScript, aParsedTokens );
	}

	// Code fuer die Argumente erzeugen
	string sArgCode;
	char sBuffer[c_iMaxBuffer];

	sprintf( sBuffer, "int argc = %d;\n", aArgs.m_aArgumentList.size() ); 
	sArgCode += sBuffer;
	if( aArgs.m_aArgumentList.size()>0 )
	{
		sprintf( sBuffer, "string argv[%d];\n", aArgs.m_aArgumentList.size() ); 
		sArgCode += sBuffer;

		int nCount = 0;
		StringContainerT::const_iterator aIter = aArgs.m_aArgumentList.begin();
		while( aIter != aArgs.m_aArgumentList.end() )
		{
			sprintf( sBuffer, "argv[%d] = \"%s\";\n", nCount, (*aIter).c_str() ); 
			sArgCode += sBuffer;
			++aIter;
			++nCount;
		}
	}

	// Block um das Script setzen
	sScript = string( "{ " ) + sArgCode + sScript + string( " }" );

	// nur wenn das Skript auch ausgefuehrt werden soll an dieser Stelle
	// (mit dem Precode und den Argumenten-Code) den Preprocessor aufrufen
	if( aArgs.m_bRunPreprocessor && aArgs.m_bRunScript )
	{
		RunPreproc( /*bOnlyPreproc*/false, aArgs, aIp, sScript, aParsedTokens );
	}

	if( aArgs.m_bRunCodegen )
	{
#ifdef USEBIG
		string sCppCodeOut;
		bool bOk = aIp.GenerateCppCode( sScript, sCppCodeOut ); 
		if( bOk )
		{
			if( aArgs.m_sOutputFileName.length()>0 )
			{
				if( !WriteAsciiFile( aArgs.m_sOutputFileName.c_str(), sCppCodeOut ) )
				{
					cerr << "error writing output file " << aArgs.m_sOutputFileName.c_str() << "." << endl;
				}
			}
			else
			{
				cout << sCppCodeOut.c_str() << endl;
			}
		}
		else
		{
			cerr << "error generating C++ code for script." << endl;
		}
#endif
	}
	else if( aArgs.m_bMakeStubs || aArgs.m_bMakeWrapper )
	{
#ifdef USEBIG
		bool bOk = aIp.ParseOnly( sScript ); 
		if( bOk )
		{
			minParserItemList aItemList, aClassItemList;

			if( aIp.GetAllFunctionPrototypes( aItemList ) && aIp.GetAllClassPrototypes( aClassItemList ) )
			{
				if( aArgs.m_bMakeStubs )
				{
					MakeStubFiles( aArgs, aItemList, aClassItemList );
				}
				if( aArgs.m_bMakeWrapper )
				{
					MakeWrapperFiles( aArgs, aClassItemList );
				}
			}
			else
			{
				cerr << "no function or class prototypes found" << endl;
			}
		}
		else
		{
			cerr << "error parsing script." << endl;
		}
#endif
	}
	else if( aArgs.m_bRunScript )
	{
		minInterpreterValue aVal;
		unsigned long nExecutionTime;
		unsigned long nParseTime;
		bool bOk = aIp.Run( sScript, aVal, &nExecutionTime, &nParseTime, aParsedTokens );
		if( bOk )
		{
			// Returnwert zuweisen
			nRet = aVal.GetInt();

			if( aArgs.m_bProfile || aArgs.m_bShowDebug )
			{
				cout << "EXECUTING (time=" << nExecutionTime << "ms parse=" << nParseTime << "ms): result=" << aVal.GetDouble() << " Strg=" << aVal.GetString() << endl;
			}
		}
		else
		{
			cerr << "error executing script " << aIp.GetErrorCode() << endl; 
		}
	}
	else if( aArgs.m_bParseOnly )
	{
		bool bOk = aIp.ParseOnly( sScript ); 
		if( bOk )
		{
			cout << "parsing script ok." << endl;
			if( aArgs.m_bDump )
			{
				SMALL( aIp.DumpParser( cout ); )
			}
		}
		else
		{
			cerr << "error parsing script." << endl;
		}
	}

	}	// Block zum Speicherleck Testen
 	MEMORY_DBG( cerr << "count = " << GetValueCount() << endl; )
	MEMORY_DBG( cerr << "items = " << GetItemCount() << endl; )

	return nRet;
}
