#include <iostream>
#include <latan/AsciiFile.hpp>
#include <latan/Mat.hpp>
#include <latan/Math.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    AsciiFile F;
    DMat A(2, 3), B(3, 2);
    const string fileName = "exMat.dat";
    
    A << 1, 2, 3,
         4, 5, 6;
    
    B << 1.0, 2.5,
         4.5, 8.9,
         1.2, 3.5;
    
    // read
    cout << "A=\n"      << A                         << '\n' << endl;
    cout << "B=\n"      << B                         << '\n' << endl;
    cout << "A*B=\n"    << A*B                       << '\n' << endl;
    cout << "cos(A)=\n" << A.unaryExpr(StdMath::cos) << '\n' << endl;
    
    // write
    cout << "-- saving A*B..." << endl;
    F.open(fileName, File::Mode::append);
    F.save(A*B, "AB");
    
    return EXIT_SUCCESS;
}
