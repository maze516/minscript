
#include "cppclasstest.h"

#include "mstl_iostream.h"

void TestFcn( string s )
{
	cout << "TestFcn s=" << s.c_str() << endl;
}

#ifdef _with_class

CppClassTest::CppClassTest()
: m_i( 0 ), m_d( 0 ), m_s( "normal" )
{
	cout << "CppClass()" << endl;
}

CppClassTest::CppClassTest( int i, double d )
: m_i( i ), m_d( d ), m_s( "int double" )
{
	cout << "CppClass( int, double )" << endl;
}

CppClassTest::CppClassTest( const string & sText )
//CppClassTest::CppClassTest( string sText )
: m_i( 0 ), m_d( 0 ), m_s( sText )
{
	cout << "CppClass( const string & )" << endl;
}

CppClassTest::~CppClassTest()
{
	cout << "~CppClass()" << endl;
}

void	CppClassTest::f() const
{
	cout << "CppClassTest::f()" << endl;
}

int		CppClassTest::GetInt() const
{
	return m_i;
}

void	CppClassTest::SetInt( int i )
{
	m_i = i;
}

double	CppClassTest::GetDouble() const
{
	return m_d;
}

void	CppClassTest::SetDouble( double d )
{
	m_d = d;
}

string	CppClassTest::GetString() const
{
	return m_s;
}

void	CppClassTest::SetString( const string & s )
{
	m_s = s;
}

double	CppClassTest::Calculate( double d )
{
	return d*d+m_d;
}

int CppClassTest::TestProtected()
{
	return m_i;
}

double CppClassTest::TestPrivate( int i )
{
	m_i = 2*i;
	return m_d;
}

#endif
