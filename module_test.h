
class TestClass
{
public:
    TestClass();
    ~TestClass();

    int f(double d);
};

int test(double);

// problems with gcc 2.9.5: string nochntest(int i, string s);
string nochntest(int i, const char * s);

int reftest( string & s );
