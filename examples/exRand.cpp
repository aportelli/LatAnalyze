#include <iostream>
#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/CompiledFunction.hpp>
#include <LatAnalyze/Plot.hpp>

using namespace std;
using namespace Latan;

constexpr Index  nDraw         = 20000;
const     string stateFileName = "exRand.seed";

int main(void)
{
    random_device         rd;
    mt19937               gen(rd());
    normal_distribution<> dis;
    DVec                  gauss(nDraw);
    Plot                  p;
    Histogram             h;
    
    cout << "-- generating " << nDraw << " Gaussian random numbers..." << endl;
    FOR_VEC(gauss, i)
    {
        gauss(i) = dis(gen);
    }
    h.setFromData(gauss, -5., 5., 40);
    h.normalize();
    cout << "             median= " << h.median() << endl;
    for (double s = 1.; s < 5.; ++s)
    {
        auto ci = h.confidenceInterval(s);
        
        cout << static_cast<int>(s) << " sigma(s) interval= [";
        cout << ci.first << ", " << ci.second << "]" << endl;
    }
    p << PlotHistogram(h);
    p << PlotFunction(compile("return exp(-x_0^2/2)/sqrt(2*pi);", 1), -5., 5.);
    p.display();

    return EXIT_SUCCESS;
}
