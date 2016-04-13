#include <LatAnalyze/CompiledFunction.hpp>
#include <LatAnalyze/GslQagsIntegrator.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    string source;
    double xMin, xMax;
    
    if (argc != 4)
    {
        cerr << "usage: " << argv[0] << " <function> <xMin> <xMax>" << endl;
        
        return EXIT_FAILURE;
    }
    source = argv[1];
    xMin   = strTo<double>(argv[2]);
    xMax   = strTo<double>(argv[3]);
    
    DoubleFunction    f = compile(source, 1);
    GslQagsIntegrator integrator;
    double            result;
    
    result = integrator(f, xMin, xMax);
    cout << "function integral on [" << xMin << ", " << xMax << "] = ";
    cout << result << " +/- " << integrator.getLastError() <<endl;
    
    return EXIT_SUCCESS;
}
