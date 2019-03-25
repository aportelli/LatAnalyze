#include <LatAnalyze/Io/Io.hpp>
#include <LatAnalyze/Functional/CompiledFunction.hpp>
#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/Statistics/Random.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>

using namespace std;
using namespace Latan;

constexpr Index  size          = 8;
constexpr Index  nDraw         = 20000;
constexpr Index  nSample       = 2000;
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

    DMat       var(size, size);
    DVec       mean(size);
    DMatSample sample(nSample, size, 1);

    cout << "-- generating " << nSample << " Gaussian random vectors..." << endl;
    var   = DMat::Random(size, size);
    var  *= var.adjoint();
    mean  = DVec::Random(size); 
    RandomNormal mgauss(mean, var, rd());
    sample[central] = mgauss();
    FOR_STAT_ARRAY(sample, s)
    {
        sample[s] = mgauss();
    }
    cout << "* original variance matrix:\n" << var << endl;
    cout << "* measured variance matrix:\n" << sample.varianceMatrix() << endl;
    cout << "* original mean:\n" << mean << endl;
    cout << "* measured mean:\n" << sample.mean() << endl;

    return EXIT_SUCCESS;
}
