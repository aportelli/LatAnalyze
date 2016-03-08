#include <iostream>
#include <LatAnalyze/FitInterface.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    FitInterface f;
    
    f.addXDim("a", 3);
    f.addXDim("b", 4);
    f.addXDim("c", 3);
    f.addYDim("y");
    
    return EXIT_SUCCESS;
}
