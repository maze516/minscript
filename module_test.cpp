
#include <string>
#include <iostream>

using namespace std;

class TestClass
{
public:
    TestClass();
    ~TestClass();

    int f(double d);
};

int test(double d)
{
   return (int)d+1;
}

string nochntest(int i, const char * s)
{
    return string(s)+"abc";
}
	
int reftest( string & s )
{
    s = "ref test successfull !";
    return 7;
}


TestClass::TestClass()
{
}

TestClass::~TestClass()
{
}


int TestClass::f(double d)
{
    return (int)d;
}
