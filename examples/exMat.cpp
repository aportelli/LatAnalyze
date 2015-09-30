#include <iostream>
#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/Mat.hpp>
#include <LatAnalyze/Math.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    DMat A(2, 3), B(3, 2);
    const string fileName = "exMat.h5";
    
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
    cout << "-- saving and loading A*B using '" + fileName + "'..." << endl;
    Io::save(A*B, fileName, File::Mode::write, "AB");

    DMat C = Io::load<DMat>(fileName);
    cout << C << endl;

    return EXIT_SUCCESS;
}
