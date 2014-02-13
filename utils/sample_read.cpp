#include <iostream>
#include <latan/Dataset.hpp>
#include <latan/Io.hpp>
#include <latan/Sample.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    Dataset<DMat, AsciiFile> dataset;
    DMatSample s, r;
    RandGen g;
    
    dataset.load("man", "HVP_2_2_000_00_0_0");
    s = dataset.bootstrapMean(100, g);
    
    cout << "central value:\n"      << s[central]               << endl;
    cout << "standard deviation:\n" << s.variance().cwiseSqrt() << endl;
    
    return EXIT_SUCCESS;
}
