#include <iostream>
#include <latan/Global.h>

using namespace std;
using namespace Latan;

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
