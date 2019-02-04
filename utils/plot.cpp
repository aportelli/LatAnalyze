#include <LatCore/OptParser.hpp>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/MatSample.hpp>
#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/MinuitMinimizer.hpp>
#include <LatAnalyze/NloptMinimizer.hpp>
#include <LatAnalyze/Plot.hpp>
#include <LatAnalyze/XYSampleData.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    // parse arguments /////////////////////////////////////////////////////////
    OptParser            opt;
    bool                 parsed;
    string               plotFileName, outFileName, xName, yName, title;
    
    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file", "");
    opt.addOption("x", "xAxis", OptParser::OptType::value  , true,
                  "x-axis name", "");
    opt.addOption("y", "yAxis", OptParser::OptType::value  , true,
                  "y-axis name", "");
    opt.addOption("t", "title", OptParser::OptType::value  , true,
                  "plot title", "");
    opt.addOption("", "help"      , OptParser::OptType::trigger, true,
                   "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or (opt.getArgs().size() < 1) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0] << " <options> <.h5 file> " << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    plotFileName = opt.getArgs()[0];
    xName        = opt.optionValue("x");
    yName        = opt.optionValue("y");
    title        = opt.optionValue("t");
    // corrFileName = opt.getArgs()[1];
    // corr0FileName = opt.getArgs()[2];
    outFileName  = opt.optionValue<string>("o");
    
    // load file /////////////////////////////////////////////////////////
    DMatSample tmp;
    Index      nt;
    
    tmp     = Io::load<DMatSample>(plotFileName);
    nt      = tmp[central].rows();
    tmp     = tmp.block(0, 0, nt, 1);
    
    // plots ///////////////////////////////////////////////////////////////////

    Plot       p;
    DVec       tAxis;

    tAxis.setLinSpaced(nt, 0, nt-1);
    p << Label(xName, Axis::x);
    p << Label(yName, Axis::y);
    p << PlotRange(Axis::x, 0, nt);
    p << PlotRange(Axis::y, -0.1, 0.1);
    p << Color("rgb 'red'") << PlotData(tAxis, tmp);
    p << Caption(title);
    p.display();

    // output //////////////////////////////////////////////////////////////////
    if (!outFileName.empty())
    {
        Io::save(tmp, outFileName);
        cout << "File saved as: " << outFileName << endl;
    }

    return EXIT_SUCCESS;
}
