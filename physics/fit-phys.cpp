#include <LatAnalyze/Io.hpp>
#include "fit-phys-env.hpp"

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    // parse arguments /////////////////////////////////////////////////////////
    string paramFileName;
    
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <parameter file>" << endl;
        
        return EXIT_FAILURE;
    }
    paramFileName = argv[1];
    
    // parse XML & load data ///////////////////////////////////////////////////
    FitEnv       env;
    
    env.parseXml(paramFileName);
    env.load();
    
    XYSampleData data = env.generateData();
    
    cout << "DATA SUMMARY" << endl;
    cout << "============" << endl;
    cout << env << data << endl;
    
    return EXIT_SUCCESS;
}
