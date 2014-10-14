#include <iostream>
#include <cmath>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/MinuitMinimizer.hpp>
#include <LatAnalyze/Plot.hpp>
#include <LatAnalyze/RandGen.hpp>
#include <LatAnalyze/XYStatData.hpp>

using namespace std;
using namespace Latan;

const Index  nPoint = 20;
const double exactPar[2] = {0.5,5.0}, dx = 10.0/static_cast<double>(nPoint);

int main(void)
{
    // generate fake data
    XYStatData          data(nPoint, 1, 1);
    RandGen             rg;
    double              x_k, y_k;
    CompiledDoubleModel f(1, 2, "return p_1*exp(-x_0*p_0);");

    for (Index k = 0; k < nPoint; ++k)
    {
        x_k          = k*dx;
        y_k          = f(&x_k, exactPar) + rg.gaussian(0.0, 0.1);
        cout << x_k << " " << y_k << " " << 0.1 << endl;
        data.x(0, k) = x_k;
        data.y(0, k) = y_k;
    }
    data.yyVar(0, 0).diagonal() = DMat::Constant(nPoint, 1, 0.1*0.1);
    data.assumeXExact(0);

    // fit
    DVec init = DVec::Constant(2, 0.5);
    FitResult p;
    MinuitMinimizer minimizer(2);
    
    data.fitAllPoints();
    p = data.fit(minimizer, init, f);
    
    cout << "a= " << p(0) << " b= " << p(1)
         << " chi^2/ndof= " << p.getChi2PerDof() << endl;

    // plot result
    Plot plot;
    
    plot << LogScale(Axis::y) << PlotData(data);
    plot << Color("rgb 'blue'") << PlotFunction(p.getModel(), 0.0, 10.0);
    plot.display();
    
    return EXIT_SUCCESS;
}
