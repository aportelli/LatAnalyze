#include <iostream>
#include <cmath>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/MinuitMinimizer.hpp>
#include <LatAnalyze/RandGen.hpp>
#include <LatAnalyze/XYStatData.hpp>

using namespace std;
using namespace Latan;

const Index  nPoint = 30;
const double xErr = .01, yErr   = .1;
const double exactPar[2] = {0.5,5.0}, dx = 10.0/static_cast<double>(nPoint);

int main(void)
{
    // generate fake data
    XYStatData  data;
    RandGen     rg;
    double      x_k, y_k;
    DoubleModel f([](const double *x, const double *p)
                  {return p[1]*exp(-x[0]*p[0]);}, 1, 2);
    
    data.addXDim("x", nPoint);
    data.addYDim("y");
    for (Index k = 0; k < nPoint; ++k)
    {
        x_k          = k*dx + rg.gaussian(0.0, xErr);
        y_k          = f(&x_k, exactPar) + rg.gaussian(0.0, yErr);
        printf("% 8e % 8e % 8e % 8e\n", x_k, xErr, y_k, yErr);
        data.x(k) = x_k;
        data.y(k) = y_k;
    }
    cout << endl;
    data.setXError(0, DVec::Constant(nPoint, xErr));
    data.setYError(0, DVec::Constant(nPoint, yErr));
    cout << data << endl;
    
    // fit
    DVec init = DVec::Constant(2, 0.5);
    FitResult p;
    MinuitMinimizer minimizer;
    
    minimizer.setVerbosity(MinuitMinimizer::Verbosity::Debug);
    p = data.fit(minimizer, init, f);
    cout << "a= " << p(0) << " b= " << p(1);
    cout << " chi^2/ndof= " << p.getChi2PerDof();
    cout << " p-value= " << p.getPValue() <<endl;
    
    return EXIT_SUCCESS;
}
