#include <iostream>
#include <cmath>
#include <latan/MinuitMinimizer.hpp>
#include <latan/Plot.hpp>
#include <latan/RandGen.hpp>
#include <latan/XYStatData.hpp>

using namespace std;
using namespace Latan;

const Index  nPoint = 20;
const double exactPar[2] = {0.5,5.0}, dx = 10.0/static_cast<double>(nPoint);

int main(void)
{
    // generate fake data
    XYStatData data(nPoint, 1, 1);
    RandGen    rg;
    double     x_k;
    auto       f = [](const double x[1], const double p[2])
                     {return p[1]*exp(-x[0]*p[0]);};
    
    for (Index k = 0; k < nPoint; ++k)
    {
        x_k          = k*dx;
        data.x(0, k) = x_k;
        data.y(0, k) = f(&x_k, exactPar) + rg.gaussian(0.0, 0.1);
    }
    data.yyVar(0, 0).diagonal() = DMat::Constant(nPoint, 1, 0.1*0.1);
    data.assumeXExact(0);
    
    // fit
    DVec init = DVec::Constant(2, 0.5);
    
    DoubleModel model(1, 2, f);
    FitResult p;
    MinuitMinimizer minimizer;
    
    data.fitAllPoints();
    p = data.fit(model, minimizer, init, true, Minimizer::Verbosity::Normal);
    
    cout << "a= " << p(0) << " b= " << p(1)
         << " chi^2/ndof= " << p.getChi2PerDof() << endl;

    // plot result
    Plot plot;
    
    plot << LogScale(Axis::y) << PlotData(data);
    plot << Color("rgb 'blue'") << PlotFunction(p.getModel(), 0.0, 10.0);
    plot.display();
    
    return EXIT_SUCCESS;
}
