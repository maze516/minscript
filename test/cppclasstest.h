
#ifndef _CPPCLASSTEST_H
#define _CPPCLASSTEST_H

#ifndef __MINSCRIPT__
#include "mstl_string.h"
#endif


void TestFcn( string s );

#define _with_class
#ifdef _with_class

class CppClassTest
{
public:
	CppClassTest();
	CppClassTest( int i, double d );
	CppClassTest( const string & sText );		// --> noch Problem !
	//CppClassTest( string sText );
	~CppClassTest();

	void	f() const;

	int		GetInt() const;
	void	SetInt( int i );

	double	GetDouble() const;
	void	SetDouble( double d );

	string	GetString() const;
	void	SetString( const string & s );

	double	Calculate( double d );

protected:
	int TestProtected();

private:
	double TestPrivate( int i );

	// data
	int		m_i;
	double	m_d;
	string	m_s;
};

#endif

#endif


