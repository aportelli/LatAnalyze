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
    bool                 parsed, doPlot, fold;
    string               corrFileName, corr0FileName, outFileName;
    Index                shift;
    
    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file", "");
    opt.addOption("s", "shift"    , OptParser::OptType::value  , true,
                  "time variable shift", "0");
    opt.addOption("p", "plot"     , OptParser::OptType::trigger, true,
                  "show the fit plot");
    opt.addOption("" , "fold"   , OptParser::OptType::trigger, true,
                  "fold the correlator");
    opt.addOption("", "help"      , OptParser::OptType::trigger, true,
                   "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or (opt.getArgs().size() < 2) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0] << " <options> < QED correlator file> < QCD correlator file 2>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    corrFileName = opt.getArgs()[0];
    corr0FileName = opt.getArgs()[1];
    outFileName  = opt.optionValue<string>("o");
    shift        = opt.optionValue<Index>("s");
    doPlot       = opt.gotOption("p");
    fold         = opt.gotOption("fold");
    
    // load correlator /////////////////////////////////////////////////////////
    DMatSample tmp0, tmp, c0, dc, effmass;
    Index      nSample, nt;
    float      tp,tm;
    
    tmp0    = Io::load<DMatSample>(corr0FileName);
    tmp     = Io::load<DMatSample>(corrFileName);
    nSample = tmp.size();
    nt      = tmp[central].rows();
    tmp0    = tmp0.block(0, 0, nt, 1);
    tmp     = tmp.block(0, 0, nt, 1);
    c0      = tmp0;
    dc      = tmp;    
    effmass = tmp; 
    FOR_STAT_ARRAY(c0, s)
    {
        for (Index t = 0; t < nt; ++t) 
        {
            c0[s]((t - shift + nt)%nt) = tmp0[s](t);
            dc[s](t) = tmp[s](t);
        }
    }
    if (fold)
    {
        cout << "Folding correlators..." << endl;
        tmp0 = c0;
        tmp  = dc;
        FOR_STAT_ARRAY(c0, s)
        {
            for (Index t = 0; t < nt; ++t)
            {
                c0[s](t) = 0.5*(tmp0[s](t) + tmp0[s]((nt - t) % nt));
            }
        }
        FOR_STAT_ARRAY(dc, s)
        {
            for (Index t = 0; t < nt; ++t)
            {
                dc[s](t) = 0.5*(tmp[s](t) + tmp[s]((nt - t) % nt));
            }
        }
    }
    FOR_STAT_ARRAY(effmass, s)
    {
        for (Index t = 0; t < nt; ++t) 
        {
            tp = (t+1)%nt;
            tm = (t-1)%nt;
            if( tm == -1)
            {
                tm = nt-1;
            }

            effmass[s](t) = ( 1./sqrt( (( c0[s](tp) + c0[s](tm) )/(2*c0[s](t)))*(( c0[s](tp) + c0[s](tm) )/(2*c0[s](t))) - 1 ) )*( (dc[s](tp) + dc[s](tm) )/(2*c0[s](t)) - ( dc[s](t)/c0[s](t) )*( ( c0[s](tp) + c0[s](tm) )/(2*c0[s](t)) ) );
        }  
    }
    // plots ///////////////////////////////////////////////////////////////////
    if(doPlot)
    {
        Plot       p;
        DVec       tAxis;

        tAxis.setLinSpaced(nt, 0, nt-1);
        p << PlotRange(Axis::x, 0, nt);
        p << PlotRange(Axis::y, -0.1, 0.1);
        p << Color("rgb 'red'") << PlotData(tAxis, effmass);
        p.display();

    }
    // output //////////////////////////////////////////////////////////////////
    if (!outFileName.empty())
    {
        Io::save(effmass, outFileName);
        cout << "File saved as: " << outFileName << endl;
    }

    return EXIT_SUCCESS;
}
