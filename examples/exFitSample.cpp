#include <iostream>
#include <cmath>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/MinuitMinimizer.hpp>
#include <LatAnalyze/RandGen.hpp>
#include <LatAnalyze/XYSampleData.hpp>

using namespace std;
using namespace Latan;

const Index  nPoint  = 30;
const Index  nSample = 1000;
const double xErr = .2, yErr   = .1;
const double exactPar[2] = {0.5,5.0}, dx = 10.0/static_cast<double>(nPoint);

int main(void)
{
    // generate fake data
    DMatSample   x(nSample, nPoint, 1), y(nSample, nPoint, 1);
    XYSampleData data(nSample);
    RandGen     rg;
    double      x1_k, x2_k;
    DoubleModel f([](const double *t, const double *p)
                  {return p[1]*exp(-t[0]*p[0]);}, 1, 2);
    
    data.addXDim("x", nPoint);
    data.addYDim("y");
    FOR_STAT_ARRAY(x, s)
    {
        for (Index k = 0; k < nPoint; ++k)
        {
            x1_k         = rg.gaussian(k*dx, xErr);
            x2_k         = rg.gaussian(k*dx, xErr);
            data.x(k)[s] = x1_k;
            data.y(k)[s] = rg.gaussian(f(&x2_k, exactPar), yErr);
        }
    }
    cout << data << endl;
    
    // fit
    DVec init = DVec::Constant(2, 0.5);
    DMat err;
    SampleFitResult p;
    MinuitMinimizer minimizer;
    
    p   = data.fit(minimizer, init, f);
    err = p.variance().cwiseSqrt();
    cout << "a= " << p[central](0) << " +/- " << err(0) << endl;
    cout << "b= " << p[central](1) << " +/- " << err(1) << endl;
    cout << "chi^2/ndof= " << p.getChi2PerDof() << endl;
    cout << "p-value= " << p.getPValue() << endl;
    
    return EXIT_SUCCESS;
}
