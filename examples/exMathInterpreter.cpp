#include <iostream>
#include <latan/Math.hpp>
#include <latan/MathInterpreter.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    string source;
    
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <program>" << endl;
        
        return EXIT_FAILURE;
    }
    source = argv[1];
    
    MathInterpreter interpreter(source);
    RunContext context;
    
    cout << "-- Source code:" << endl << source << endl << endl;
    interpreter.compile();
    cout << "-- Abstract Syntax Tree:" << endl;
    if (interpreter.getAST())
    {
        cout << *interpreter.getAST() << endl;
    }
    else
    {
        cout << "<null>" << endl << endl;
    }
    cout << "-- Program:" << endl << interpreter << endl;
    StdMath::addStdMathFunc(context.fTable);
    interpreter(context);
    if (!context.dStack.empty())
    {
        cout << "-- Result: " << context.dStack.top() << endl;
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
