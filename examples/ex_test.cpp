#include <iostream>
#include <latan/MathCompiler.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    MathCompiler C(argv[1]);
    VarTable vtable;
    stack<double> dstack;
    const VirtualProgram& P = C();
    
    cout << P << endl;
    for (int i=0;i<P.size();++i)
    {
        (*(P[i]))(dstack,vtable);
    }
    cout << "result= " << dstack.top() << endl;
    
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
