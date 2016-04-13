#include <iostream>
#include <cmath>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/GslMinimizer.hpp>
#include <LatAnalyze/Plot.hpp>
#include <LatAnalyze/XYStatData.hpp>

using namespace std;
using namespace Latan;

const Index  nPoint1 = 10, nPoint2 = 10;
const double xErr = .1, yErr   = .3;
const double exactPar[2] = {0.5,5.};
const double dx1 = 10.0/static_cast<double>(nPoint1);
const double dx2 = 5.0/static_cast<double>(nPoint2);

int main(void)
{
    // generate fake data
    XYStatData            data;
    random_device         rd;
    mt19937               gen(rd());
    normal_distribution<> dis;
    double                xBuf[2];
    DoubleModel           f([](const double *x, const double *p)
                            {return p[1]*exp(-x[0]*p[0])+x[1];}, 2, 2);
    
    cout << "-- generating fake data..." << endl;
    data.addXDim(nPoint1);
    data.addXDim(nPoint2);
    data.addYDim();
    for (Index i1 = 0; i1 < nPoint1; ++i1)
    {
        xBuf[0]       = i1*dx1;
        data.x(i1, 0) = xErr*dis(gen) + xBuf[0];
        for (Index i2 = 0; i2 < nPoint2; ++i2)
        {
            xBuf[1]                           = i2*dx2;
            data.x(i2, 1)                     = xBuf[1];
            data.y(data.dataIndex(i1, i2), 0) = yErr*dis(gen)
                                                + f(xBuf, exactPar);
        }
    }
    data.setXError(0, DVec::Constant(data.getXSize(0), xErr));
    data.assumeXExact(true, 1);
    data.setYError(0, DVec::Constant(data.getYSize(), yErr));
    
    // set minimizers
    DVec         init = DVec::Constant(2, 0.1);
    FitResult    p;
    GslMinimizer min(GslMinimizer::Algorithm::bfgs2);
    
    // fit
    min.setVerbosity(Minimizer::Verbosity::Debug);
    cout << "-- fit..." << endl;
    f.parName().setName(0, "m");
    f.parName().setName(1, "A");
    p = data.fit(min, init, f);
    p.print();
    
    // plot
    Plot       plot;
    DVec       ref(2);
    XYStatData res;
    
    cout << "-- generating plots..." << endl;
    ref(1) = 0.;
    res    = data.getPartialResiduals(p, ref, 0);
    plot << PlotRange(Axis::x, 0., 10.);
    plot << Color("rgb 'blue'");
    plot << PlotFunction(p.getModel().bind(0, ref), 0., 10.);
    plot << Color("rgb 'red'");
    plot << PlotData(res);
    plot.display();
    
    return EXIT_SUCCESS;
}
