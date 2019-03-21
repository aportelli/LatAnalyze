#include <LatAnalyze/Functional/CompiledFunction.hpp>
#include <LatAnalyze/Core/Math.hpp>
#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/Functional/TabFunction.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    const Index nPoint = 8;
    Plot p;
    DVec x(nPoint), y(nPoint);
    
    for (Index i = 0; i < nPoint; ++i)
    {
        x(i) = static_cast<double>(i)/2.;
        y(i) = x(i)*x(i);
    }
    p << PlotRange(Axis::x, -5.0, 5.0) << PlotRange(Axis::y, -5.0, 20.0);
    p << Color("rgb 'blue'") << PlotFunction(StdMath::tgamma, -5, 5);
    p << PlotFunction(compile("return cos(x_0)^2;", 1), -5, 5);
    p << Color("rgb 'brown'") << PlotCommand("x**3");
    p << PlotCommand("x**2");
    p << PlotFunction(interpolate(x, y), 0., nPoint/2.0 - 1.1);
    cout << p << endl;
    p.display();
    
    return EXIT_SUCCESS;
}
