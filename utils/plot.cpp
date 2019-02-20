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
    vector<string>       inFileName;
    double               xLow, xHigh, spacing;

    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file", "");
    opt.addOption("x", "xAxis", OptParser::OptType::value  , true,
                  "x-axis name", "");
    opt.addOption("l", "xLow", OptParser::OptType::value  , true,
                  "x-axis lower bound", "0");
    opt.addOption("y", "yAxis", OptParser::OptType::value  , true,
                  "y-axis name", "");
    opt.addOption("s", "spacing", OptParser::OptType::value  , true,
                  "spacing between points", "1");
    opt.addOption("t", "title", OptParser::OptType::value  , true,
                  "plot title", "");
    opt.addOption("", "help"      , OptParser::OptType::trigger, true,
                   "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or opt.gotOption("help") or opt.getArgs().size() != 1)
    {
        cerr << "usage: " << argv[0] << " <.h5/manifest file> <options> " << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    
    plotFileName = opt.getArgs().front();
    xName        = opt.optionValue("x");
    xLow         = opt.optionValue<double>("l");
    yName        = opt.optionValue("y");
    spacing      = opt.optionValue<double>("s");
    title        = opt.optionValue("t");
    outFileName  = opt.optionValue<string>("o");


    if(plotFileName.find(".h5") == string::npos)
    {
        inFileName = readManifest(plotFileName);
    }
    
    // load and plot file(s) /////////////////////////////////////////////////////////
    DMatSample tmp;
    Index      nt;
    Plot       p;
    DVec       tAxis;


    if(inFileName.size() == 0)
    {
        tmp     = Io::load<DMatSample>(plotFileName);
        nt      = tmp[central].rows();
        tmp     = tmp.block(0, 0, nt, 1);
        xHigh= xLow+spacing*(nt-1);
        tAxis.setLinSpaced(nt, xLow, xHigh);
        p  << PlotData(tAxis, tmp); 
    
    }
    else
    {
        tmp = Io::load<DMatSample>(inFileName[0]);
        nt      = tmp[central].rows();
        tmp     = tmp.block(0, 0, nt, 1);
        xHigh= xLow+spacing*(nt-1);
        tAxis.setLinSpaced(nt, xLow, xHigh);

        for(unsigned long i = 0; i < inFileName.size(); i++)
        {
            plotFileName = inFileName[i];
            tmp = Io::load<DMatSample>(plotFileName);
            tmp     = tmp.block(0, 0, nt, 1);
            
            p  << PlotData(tAxis, tmp);
            
        }
        
    }

    p << Label(xName, Axis::x);
    p << Label(yName, Axis::y);
    p << PlotRange(Axis::x, xLow, xHigh);
    p << Caption(title);
    
    cout << "Displaying plot..." << endl;
    p.display();

    // output //////////////////////////////////////////////////////////////////
    if (!outFileName.empty())
    {
        Io::save(tmp, outFileName);
        cout << "File saved as: " << outFileName << endl;
    }

    return EXIT_SUCCESS;
}
