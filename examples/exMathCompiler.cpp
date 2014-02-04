#include <iostream>
#include <latan/Math.hpp>
#include <latan/MathCompiler.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    string source;
    
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <program>" << endl;
    }
    source = argv[1];
    
    MathCompiler C(source);
    VarTable vtable;
    FunctionTable ftable;
    stack<double> dstack;
    const VirtualProgram& P = C();
    
    cout << "-- Source code:" << endl << source << endl << endl;
    cout << "-- Abstract Syntax Tree:" << endl << *C.getAST() << endl;
    cout << "-- Program:" << endl << P << endl;
    StdMath::addStdMathFunc(ftable);
    for (unsigned int i=0;i<P.size();++i)
    {
        (*(P[i]))(dstack, vtable, ftable);
    }
    if (!dstack.empty())
    {
        cout << "-- Result: " << dstack.top() << endl;
    }
    
    return EXIT_SUCCESS;
}

/*int main(void)
{
    ASCIIFile F;
    DMat A,B;
    
    F.Open("foo.boot",FileMode::Read);
    A = F.Read<DMat>("bla");
    B = F.Read<DMat>("bli");
    cout << A << endl;
    cout << B << endl;
    cout << A*B << endl;

    return EXIT_SUCCESS;
}*/

/*
int main(void)
{
    DMat m(2,2);
    
    m(0,6) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    cout << "Here is the matrix m:\n" << m << endl;
    DVec v(2);
    v(0) = 4;
    v(1) = v(0) - 1;
    cout << "Here is the vector v:\n" << v << endl;
}
*/
