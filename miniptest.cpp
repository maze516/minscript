

#define _USE_DLL
#include "dllexport.h"

#include "minparsernodes.h"
#include "minip.h"

#include "mstl_string.h"
#include "mstl_iostream.h"

#include <stdio.h>			// fuer: sprintf()

//*************************************************************************
//*************************************************************************
//*************************************************************************

void ExecuteScript( const char * sScriptStrg, bool bDebug )
{
	minScriptInterpreter aIp( bDebug );
	string sScript = "{ "+string(sScriptStrg)+" }";

	minInterpreterValue aVal;
	unsigned long nExecutionTime;
	bool bOk = aIp.Run( sScript, aVal, &nExecutionTime );
	if( bOk )
	{
		cerr << "EXECUTING (time=" << nExecutionTime << "ms): result=" << aVal.GetDouble() << " Strg=" << aVal.GetString() << endl;
	}
	else
	{
		cerr << "ERROR executing script " << aIp.GetErrorCode() << endl; 
	}
}

void ConvertScript( const char * sScriptStrg )
{
	minScriptInterpreter aIp;
	string sScript = "{ "+string(sScriptStrg)+" }";
	string sCppCodeOut;

	minInterpreterValue aVal;
	bool bOk = aIp.GenerateCppCode( sScript, sCppCodeOut ); 
	cout << "Converting Script to C++-Code: " << (bOk ? "ok" : "error") << endl;
	if( bOk )
	{
		cout << "Result:" << endl << sCppCodeOut.c_str() << endl;
	}
}

void RunPreProcessing( const char * sScriptStrg )
{
	minScriptInterpreter aIp;

	string sScript( sScriptStrg );
	string sPreProcedScript;
	aIp.RunPreProcessor( sScript, sPreProcedScript );
	cout << sPreProcedScript << endl;
}

void ParseScript( const char * sScriptStrg )
{
	minScriptInterpreter aIp;
	string sScript = "{ "+string(sScriptStrg)+" }";
	string sCppCodeOut;

	bool bOk = aIp.ParseOnly( sScript ); 
	cerr << "Parsing Script, result=" << (bOk ? "ok" : "error") << endl;
}

void DumpParseScript( const char * sScriptStrg )
{
	minScriptInterpreter aIp;
	string sScript = "{ "+string(sScriptStrg)+" }";
	string sCppCodeOut;

	cout << "Dump Parser Tree:" << endl;
	bool bOk = aIp.ParseOnly( sScript ); 
	if( bOk )
	{
		aIp.DumpParser( cout );
	}
}

void SpeedTestInterpreter()
{
	minScriptInterpreter aIp;
	//string sScript = "{ int f( int i ) { return i*i; } int i; int j; i=0; while( i<10000 ) { i = i+1; j=f(i); } j; }";
	// auf K6 233 MHz: Script mit MS C++: DEBUG: 1210ms; RELEASE=460ms
	// auf K6 233 MHz: C++ Code direkt: DEBUG: 90ms; RELEASE=40ms fuer die 100 fache Durchlauefe (1000000)

	string sScript = "{ int f( int i ) { return i*i; } int i; int j; i=0; while( i<10000 ) { i = i+1; j=i; } j; }";
	// auf K6 233 MHz: Script mit MS C++: DEBUG: ? ms; RELEASE=190ms/390ms/771ms
	//														   *1	/*2   /*4

	minInterpreterValue aVal;
	bool bOk = aIp.Run( sScript, aVal );
	if( bOk )
	{
		cout << "EXECUTING: result=" << aVal.GetDouble() << " Strg=" << aVal.GetString() << endl;
	}
	else
	{
		cout << "ERROR executing script " << aIp.GetErrorCode() << endl; 
	}
}

void TestInterpreter()
{
	minScriptInterpreter aIp;
	//string sScript = "{ 1/2; a = 4; 1+2; 3*4+2; 1+(3+4)*2+4; b=7; c=b*a; c; }";
	//string sScript = "{ i = 10; s = 0; while( i ) i=i-1; i; /*{ i = i-1; s=s+2; } s;*/ }";
	//string sScript = "{ i = 10; s = 0; while( i ) { i = i-1; s=s+2; } s; if( s ) strg=\"hallo\"; else strg=\"welt\"; strg; }";
	//string sScript = "{ i = 10; s = 0; for( i=0; i<10; i=i+1 ) { s=s+1; } s; }";
	//string sScript = "{ i = 10; s = 0; do { i = i-1; s=s+2; } while( i>4 ); i++; i; ; }";
	//string sScript = "{ int i; i = 0; !i; }";
	string sScript = "{ int f( int i, float dd ) { return dd*i; } int g() { return 3*f(4,6); } int i; i = 2; i = i*f(5,5*4)-2*g(); i=9; sin(); f(5,9+1); }";
	string sCppCode;

	minInterpreterValue aVal;
	bool bOk = aIp.Run( sScript, aVal );
	if( bOk )
	{
		cout << "EXECUTING: result=" << aVal.GetDouble() << " Strg=" << aVal.GetString() << endl;
	}
	else
	{
		cout << "ERROR executing script " << aIp.GetErrorCode() << endl; 
	}

	aIp.GenerateCppCode( sScript, sCppCode );
	cout << "C++ Code:" << endl;
	cout << sCppCode << endl;
}

void TestTokenizer()
{
	minTokenizer aTokenizer;

	cout << "Testing Tokenizer" << endl;

	aTokenizer.AddToken( minToken( Whitespace, " " ) );
	aTokenizer.AddToken( minToken( Whitespace, "\t" ) );
	aTokenizer.AddToken( minToken( Whitespace, "\n" ) );
	aTokenizer.AddToken( minToken( Whitespace, "\r" ) );

	aTokenizer.AddToken( minToken( SpecialChar, "{", BLOCK_OPEN_ID ) );
	aTokenizer.AddToken( minToken( SpecialChar, "}", BLOCK_CLOSE_ID ) );
	aTokenizer.AddToken( minToken( SpecialChar, "(", PARENTHIS_OPEN_ID ) );
	aTokenizer.AddToken( minToken( SpecialChar, ")", PARENTHIS_CLOSE_ID ) );
	aTokenizer.AddToken( minToken( SpecialChar, "." ) );
	aTokenizer.AddToken( minToken( SpecialChar, ";", STATEMENT_END_ID ) );

	aTokenizer.AddToken( minToken( Operator, "++" ) );
	aTokenizer.AddToken( minToken( Operator, "+", ADD_ID ) );
	aTokenizer.AddToken( minToken( Operator, "--" ) );
	aTokenizer.AddToken( minToken( Operator, "-", SUB_ID ) );
	aTokenizer.AddToken( minToken( Operator, "*", MULT_ID ) );
	aTokenizer.AddToken( minToken( Operator, "/", DIV_ID ) );
	aTokenizer.AddToken( minToken( Operator, "=", ASSIGN_ID ) );

	aTokenizer.AddToken( minToken( Keyword, "if" ) );
	aTokenizer.AddToken( minToken( Keyword, "else" ) );
	aTokenizer.AddToken( minToken( Keyword, "for" ) );
	aTokenizer.AddToken( minToken( Keyword, "while" ) );
	aTokenizer.AddToken( minToken( Keyword, "do" ) );

//	aTokenizer.SetText( "if(bTest)+12.345e-456-42.1*1.234e+12 / true + \"hallo \\n \\\"welt\" -- /*Kommentar*/ -a3c g_var // c++\n abc" );
	aTokenizer.SetText( "1+(3+4)*2+4; 1/2; a = 4; 1+2; 3*4+2;" );

	minParser aParser( &aTokenizer );

	aParser.Parse();

	if( aTokenizer.InitProcessing() )
	{
		minToken aToken;

		while( aTokenizer.GetNextToken( aToken ) )
		{
			cout << "Type=" << aToken.GetType() << " rep=" << aToken.GetRepresentationString() << endl;
		}

		if( aTokenizer.IsError() )
		{
			cout << "ACHTUNG: Error in tokenizing string" 
				 << " ErrCode=" << aTokenizer.GetErrorCode() 
				 << " Pos=" << aTokenizer.GetErrorPos() << endl;
		}
	}

	cout << "Testing Tokenizer done." << endl;
}

// fuer ddd unter linux...
int _argc = 2;
char *_argv[] = { "minscript.exe" , "-f=x.ic" };

// Funktion zum scannen von Komandozeilen-Parametern
void DoProcessArguments
( 
	int m_argc, char *m_argv[],  
	string & sScript, 
	bool & bDoExecute, 
	bool & bDoConvert, 
	bool & bDoVerbose, 
	bool & bDoDebug, 
	bool & bDoDump, 
	bool & bDoPreProcessing 
)
{
	char sBuffer[4096];
	int nDoParseOnly = 0;
	int nDoConvert = 0;
	int nDoVerbose = 0;
	int nDoDebug = 0;
	int nDoDump = 0;
	int nDoPreProcessingOnly = 0;

	strcpy( sBuffer, "" );
	for( int i=1; i<m_argc; i++ )
	{
		//cout << m_argv[i]  << endl;
		if( strlen( m_argv[i] )>1 /*&& m_argv[i][2]=='='*/ && m_argv[i][0]=='-' )
  		{
			switch(m_argv[i][1])
 			{
				case 'f':
					{
						int nCount = sscanf( m_argv[i], "-f=%s", sBuffer );
						if( nCount>0 )
						{
							if( !ReadScript( sBuffer, sScript ) )
							{
								cout << "Warning: can not read " << sBuffer << endl;
							}
						}
						else
							sScript = "";
					} break;
	    		case 'p':
 	     			//sscanf( m_argv[i], "-p=%d", &nDoParseOnly );
					nDoParseOnly = 1;
  	       			break;
	    		case 'u':
 	     			//sscanf( m_argv[i], "-u=%d", &nDoDump );
					nDoParseOnly = 1;
					nDoDump = 1;
  	       			break;
	    		case 'c':
 	     			//sscanf( m_argv[i], "-c=%d", &nDoConvert );
					nDoConvert = 1;
  	       			break;
	    		case 'v':
 	     			//sscanf( m_argv[i], "-v=%d", &nDoVerbose );
					nDoVerbose = 1;
  	       			break;
	    		case 'd':
 	     			//sscanf( m_argv[i], "-d=%d", &nDoDebug );
					nDoDebug = 1;
  	       			break;
	    		case 'r':
 	     			//sscanf( m_argv[i], "-r=%d", &nDoPreProcessingOnly );
					nDoPreProcessingOnly = 1;
  	       			break;
				default: 
					sScript = m_argv[i];
			}
   		}
		else
		{
			sScript = m_argv[i];
		}
	}
	bDoExecute = (bool)!nDoParseOnly;
	bDoConvert = (bool)nDoConvert;
	bDoVerbose = (bool)nDoVerbose;
	bDoDebug = (bool)nDoDebug;
	bDoDump = (bool)nDoDump;
	bDoPreProcessing = (bool)nDoPreProcessingOnly;
}

//*************************************************************************

int main( int argc, char *argv[] )
{
	cerr << endl << "miniScript Program, Ver. 1.1 from " << __DATE__ << ",\n(c) by Michael Neuroth, 1999, 2000" << endl;
	cerr << endl << argv[0] << " [script] [-f=scriptfile] [-e(xecute)] [-p(arse)] [-c[onvert_to_cpp] "
				 << "[-v(erbose)] [-d(ebug)] [-u(dump)] [-r(preprocessing)]" << endl;
	
	/*
	for( int i=0; i<argc; ++i )
	{
		cout << i << " " << argv[i] << endl;
	}
	*/

	//TestTokenizer();
	//TestInterpreter();
	//SpeedTestInterpreter();

	{
		string sScript;
		bool bDoExecute;
		bool bDoConvert;
		bool bVerbose;
		bool bDebug;
		bool bDump;
		bool bDoPreProcessing;
		//DoProcessArguments( _argc, _argv, sScript, bDoExecute, bDoConvert, bVerbose, bDebug, bDump, bDoPreProcessing );
		DoProcessArguments( argc, argv, sScript, bDoExecute, bDoConvert, bVerbose, bDebug, bDump, bDoPreProcessing );
		if( bVerbose )
			cout << "SCRIPT:" << endl << sScript.c_str() << endl;
		if( sScript!="" )
		{
			if( bDoPreProcessing )
				/*bool bOk =*/ RunPreProcessing( sScript.c_str() );
			else if( bDoExecute )
				ExecuteScript( sScript.c_str(), bDebug );
			else if( bDump )
				DumpParseScript( sScript.c_str() );
			else
				ParseScript( sScript.c_str() );
			if( bDoConvert )
				ConvertScript( sScript.c_str() );
		}
	}

	//TestInterpreter();

	cout << endl << "done." << endl;

	return 0;
}
