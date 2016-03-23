#include <iostream>
#include <cmath>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/MinuitMinimizer.hpp>
#include <LatAnalyze/RandGen.hpp>
#include <LatAnalyze/XYStatData.hpp>

using namespace std;
using namespace Latan;

const Index  nPoint1 = 5, nPoint2 = 5;
const double xErr = .1, yErr   = .1;
const double exactPar[2] = {0.5,5.};
const double dx1 = 10.0/static_cast<double>(nPoint1);
const double dx2 = 5.0/static_cast<double>(nPoint2);

int main(void)
{
    // generate fake data
    XYStatData  data;
    RandGen     rg;
    double      xBuf[2];
    DoubleModel f([](const double *x, const double *p)
                  {return p[1]*exp(-x[0]*p[0])+x[1];}, 2, 2);
    
    data.addXDim("x", nPoint1);
    data.addXDim("off", nPoint2);
    data.addYDim("y");
    for (Index i1 = 0; i1 < nPoint1; ++i1)
    {
        xBuf[0]       = i1*dx1;
        data.x(i1, 0) = rg.gaussian(xBuf[0], xErr);
        for (Index i2 = 0; i2 < nPoint2; ++i2)
        {
            xBuf[1]                        = i2*dx2;
            data.x(i2, 1)                  = xBuf[1];
            data.y(data.dataIndex(i1, i2)) = rg.gaussian(f(xBuf, exactPar),
                                                         yErr);
            printf("% 8e % 8e % 8e % 8e % 8e\n", data.x(i1, 0), xErr,
                   data.x(i2, 1), data.y(i1), yErr);
        }
    }
    cout << endl;
    data.setXError(0, DVec::Constant(data.getXSize(0), xErr));
    data.assumeXExact(true, 1);
    data.setYError(0, DVec::Constant(data.getYSize(), yErr));
    cout << data << endl;
    
    // fit
    DVec init = DVec::Constant(2, 0.1);
    FitResult p;
    MinuitMinimizer minimizer;
    
    minimizer.setVerbosity(Minimizer::Verbosity::Normal);
    p = data.fit(minimizer, init, f);
    cout << "a= " << p(0) << " b= " << p(1) << endl;
    cout << "chi^2/ndof= " << p.getChi2PerDof() << endl;
    cout << "p-value= " << p.getPValue() <<endl;
    
    return EXIT_SUCCESS;
}
