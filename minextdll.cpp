
#include "proto.ic"

#define _USE_DLL
#include "dllexport.h"
#include "minnativehlp.h"
#include "minip.h"

extern "C" bool RegisterNativeFunctions( minInterpreterEnvironment & aEnvironment )
{
	minNativeFcnInterface * pFcn;

	pFcn = new NativeFcnWrapper1<int,int>( _f, "int _f( int );" );
	aEnvironment.AddFunction( minHandle<minFunctionDeclarationNode>( new minNativeFunctionDeclarationNode( pFcn ) ) );
	pFcn = new NativeFcnWrapper1<int,int&>( _g, "int _g( int& );" );
	aEnvironment.AddFunction( minHandle<minFunctionDeclarationNode>( new minNativeFunctionDeclarationNode( pFcn ) ) );
	pFcn = new NativeVoidFcnWrapper2<double,int>( _h, "void _h( double, int );" );
	aEnvironment.AddFunction( minHandle<minFunctionDeclarationNode>( new minNativeFunctionDeclarationNode( pFcn ) ) );
	pFcn = new NativeVoidFcnWrapper0( _i, "void _i(  );" );
	aEnvironment.AddFunction( minHandle<minFunctionDeclarationNode>( new minNativeFunctionDeclarationNode( pFcn ) ) );

	return true;
}
