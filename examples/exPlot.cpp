#include <iostream>
#include <LatAnalyze/CompiledFunction.hpp>
#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/Plot.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    Plot p;
    
    p << PlotRange(Axis::x, -5.0, 5.0) << PlotRange(Axis::y, -5.0, 5.0);
    p << Color("rgb 'blue'") << PlotFunction(StdMath::tgamma, -5, 5);
    p << PlotFunction(CompiledDoubleFunction(1, "return cos(x_0)^2;"), -5, 5);
    p << Color("rgb 'brown'") << PlotCommand("x**3");
    cout << p << endl;
    p.display();
    
    return EXIT_SUCCESS;
}
