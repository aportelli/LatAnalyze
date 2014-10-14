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
    
    CompiledDoubleFunction f(1, source);
    MinuitMinimizer minimizer(1);
    double min;
    
    minimizer.setVerbosity(Minimizer::Verbosity::Debug);
    min = minimizer(f)(0);
    cout << "function minimum = " << min << endl;
    
    return EXIT_SUCCESS;
}
