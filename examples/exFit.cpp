#include <iostream>
#include <LatAnalyze/XYStatData.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    XYStatData f;
    
    f.addYDim("q1");
    f.addYDim("q2");
    f.addXDim("x1", 6);
    f.addXDim("x2", 5);
    f.addXDim("x3", 5);
    f.y(f.dataIndex(0,0,0), 0) = 2;
    f.y(f.dataIndex(1,1,1), 0) = 4;
    f.y(f.dataIndex(2,2,2), 0) = 5;
    f.y(f.dataIndex(2,3,3), 0) = 10;
    f.y(f.dataIndex(0,0,0), 1) = 1;
    f.y(f.dataIndex(1,1,1), 1) = 2;
    f.y(f.dataIndex(2,2,3), 1) = 4;
    f.fitPoint(false, f.dataIndex(2,2,2), 0);
    f.fitPoint(false, f.dataIndex(1,1,1), 1);
    f.assumeXXCorrelated(true, 0, 0, 1, 0);
    f.assumeXXCorrelated(true, 0, 1, 1, 1);
    f.assumeXXCorrelated(true, 0, 2, 1, 2);
    f.assumeXXCorrelated(true, 0, 0, 1, 2);
    f.assumeXXCorrelated(true, 3, 2, 4, 2);
    f.assumeYYCorrelated(true, f.dataIndex(0,0,0), 0, f.dataIndex(2,3,3), 0);
    f.assumeYYCorrelated(true, f.dataIndex(0,0,0), 1, f.dataIndex(2,2,3), 1);
    f.assumeXYCorrelated(true, 0, 0, f.dataIndex(1,1,1), 0);
    f.assumeXExact(true, 0);
    f.assumeXExact(true, 1);
    f.assumeXExact(true, 2);
    cout << f << endl;
    f.setXXVar(0, 0, DMat::Identity(6, 6));
    f.setXXVar(0, 2, DMat::Identity(6, 5));
    f.setXXVar(1, 1, DMat::Identity(5, 5));
    f.setXXVar(2, 2, DMat::Identity(5, 5));
    DEBUG_MAT(f.makeCorrFilter());
    DEBUG_MAT(f.getFitVar());
    DEBUG_MAT(f.getFitVar().cwiseProduct(f.makeCorrFilter()));
    
    return EXIT_SUCCESS;
}
