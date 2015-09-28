#include <iostream>
#include <LatAnalyze/AsciiFile.hpp>
#include <LatAnalyze/CompiledFunction.hpp>
#include <LatAnalyze/Plot.hpp>
#include <LatAnalyze/RandGen.hpp>

using namespace std;
using namespace Latan;

constexpr int    seqLength     = 25;
constexpr int    saveStep      = 9;
constexpr Index  nDraw         = 20000;
const     string stateFileName = "exRand.seed";

int main(void)
{
    RandGenState state;
    RandGen      gen[2];
    AsciiFile    stateFile(stateFileName, File::Mode::write|File::Mode::read);
    DVec         gauss(nDraw);
    Plot         p;
    Histogram    h;
    
    cout << "- GENERATOR STATE I/O TESTS" << endl;
    cout << "-- generating a " << seqLength << " steps random sequence..."
         << endl;
    for (int i = 0; i < seqLength; ++i)
    {
        if (i == saveStep)
        {
            state = gen[0].getState();
            stateFile.save(state, "exRand");
            cout << "generator state after step " << saveStep - 1
                 << " saved in '" << stateFileName << "'" << endl;
        }
        cout << "step " << i << "\t: " << gen[0].uniform() <<endl;
    }
    cout << "-- setting up another generator from '" << stateFileName << "'..."
         << endl;
    gen[1].setState(stateFile.read<RandGenState>("exRand"));
    cout << "-- generating a " << seqLength << " steps random sequence..."
         << endl;
    for (int i = 0; i < seqLength; ++i)
    {
        cout << "step " << i << "\t: " << gen[1].uniform() <<endl;
    }
    cout << "-- generating " << nDraw << " Gaussian random numbers..." << endl;
    FOR_VEC(gauss, i)
    {
        gauss(i) = gen[0].gaussian();
    }
    h.setFromData(gauss, -5., 5., 40);
    h.normalize();
    p << PlotHistogram(h);
    p << PlotFunction(compile("return exp(-x_0^2/2)/sqrt(2*pi);", 1), -5., 5.);
    p.display();

    return EXIT_SUCCESS;
}
