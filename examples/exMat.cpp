#include <iostream>
#include <latan/Io.hpp>
#include <latan/Mat.hpp>
#include <latan/Math.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    AsciiFile F;
    DMat A,B;
    const string fileName = "exMat.dat";
    
    // read
    cout << "-- reading " << fileName << "..." << endl;
    F.open(fileName, File::Mode::read);
    A = F.read<DMat>("A");
    B = F.read<DMat>("B");
    cout << "A=\n"      << A                         << '\n' << endl;
    cout << "B=\n"      << B                         << '\n' << endl;
    cout << "A*B=\n"    << A*B                       << '\n' << endl;
    cout << "cos(A)=\n" << A.unaryExpr(StdMath::cos) << '\n' << endl;
    F.close();
    
    // write
    cout << "-- saving A*B..." << endl;
    F.open(fileName, File::Mode::append);
    F.save(A*B, "AB");
    
    return EXIT_SUCCESS;
}
