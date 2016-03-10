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
    f.assumeXXCorrelated(true, 0, 0, 0, 1);
    f.assumeXXCorrelated(true, 1, 1, 0, 1);
    f.assumeXXCorrelated(true, 2, 2, 0, 1);
    f.assumeYYCorrelated(true, 0, 0, f.dataIndex(0,0,0), f.dataIndex(1,1,1));
    f.assumeYYCorrelated(true, 1, 1, f.dataIndex(0,0,0), f.dataIndex(2,2,3));
    f.assumeXYCorrelated(true, 0, 0, 0, f.dataIndex(1,1,1));
    cout << f << endl;
    f.updateLayout();
    DEBUG_MAT(f.makeCorrFilter());
    
    return EXIT_SUCCESS;
}
