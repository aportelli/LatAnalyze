#include <iostream>
#include <LatAnalyze/FitInterface.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    FitInterface f;
    
    f.addYDim("q1");
    f.addYDim("q2");
    f.addXDim("x1", 6);
    f.addXDim("x2", 5);
    f.addXDim("x3", 5);
    f.registerDataPoint(f.dataIndex(0,0,0), 0);
    f.registerDataPoint(f.dataIndex(1,1,1), 0);
    f.registerDataPoint(f.dataIndex(2,2,2), 0);
    f.registerDataPoint(f.dataIndex(2,3,3), 0);
    f.registerDataPoint(f.dataIndex(0,0,0), 1);
    f.registerDataPoint(f.dataIndex(1,1,1), 1);
    f.registerDataPoint(f.dataIndex(2,2,3), 1);
    f.fitPoint(false, f.dataIndex(1,1,1), 1);
    cout << f << endl;
    DEBUG_VAR(f.getYFitSize());
    DEBUG_VAR(f.getYFitSize(0));
    DEBUG_VAR(f.getYFitSize(1));
    DEBUG_VAR(f.getXFitSize());
    DEBUG_VAR(f.getXFitSize(0));
    DEBUG_VAR(f.getXFitSize(1));
    DEBUG_VAR(f.getXFitSize(2));
    
    return EXIT_SUCCESS;
}
