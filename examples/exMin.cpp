#include <iostream>
#include <LatAnalyze/CompiledFunction.hpp>
#include <LatAnalyze/MinuitMinimizer.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    string source;
    
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <function>" << endl;
        
        return EXIT_FAILURE;
    }
    source = argv[1];
    
    DoubleFunction  f = compile(source, 1);
    MinuitMinimizer minimize(1);
    DVec init(1);
    double min;

    init(0) = 0.1;
    minimize.setInit(init);
    minimize.setVerbosity(Minimizer::Verbosity::Debug);
    min = minimize(f)(0);
    cout << "function minimum = " << min << endl;
    
    return EXIT_SUCCESS;
}
