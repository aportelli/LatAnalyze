#include <iostream>
#include <iomanip>
#include <LatAnalyze/CompiledFunction.hpp>

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
    
    cout << "-- Program:" << endl << f << endl;
    cout << "-- Values:" << endl;
    for (double x = 0.0; x < 10.0; x += 0.5)
    {
        cout << "f(" << right << setw(6) << strFrom<double>(x) << ")= "
             << f(x) << endl;
    }
    
    return EXIT_SUCCESS;
}
