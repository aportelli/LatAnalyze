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
    bool                 parsed, doPlot;
    string               corrFileName, corr0FileName, outFileName;
    Index                shift;
    
    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file", "");
    opt.addOption("s", "shift"    , OptParser::OptType::value  , true,
                  "time variable shift", "0");
    opt.addOption("p", "plot"     , OptParser::OptType::trigger, true,
                  "show the fit plot");
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
    
    // load correlator /////////////////////////////////////////////////////////
    DMatSample tmp, c0, dc, effmass;
    Index      nSample, nt;
    float      tp,tm;
    
    tmp     = Io::load<DMatSample>(corr0FileName);
    nSample = tmp.size();
    nt      = tmp[central].rows();
    tmp     = tmp.block(0, 0, nt, 1);
    c0   = tmp;
    dc   = tmp;
    effmass = tmp; // initialise effmass like this
    FOR_STAT_ARRAY(c0, s) // loads the QCD correlator, bootstrap sample by sample
    {
        for (Index t = 0; t < nt; ++t) 
        {
            c0[s]((t - shift + nt)%nt) = tmp[s](t);
        }
    }
    tmp     = Io::load<DMatSample>(corrFileName);
    tmp     = tmp.block(0, 0, nt, 1);
    FOR_STAT_ARRAY(dc, s) // computes the leading order perturbation in corr
    {
        for (Index t = 0; t < nt; ++t) 
        {
            dc[s](t) = tmp[s](t);
        }
    }
    FOR_STAT_ARRAY(effmass, s) //generate effective mass here
    {
        for (Index t = 0; t < nt; ++t) 
        {
            tp = (t+1)%nt;
            tm = (t-1)%nt;
            if( tm == -1)
            {
                tm = nt-1;
            }

            effmass[s](t) = ( 1./sqrt( ( c0[s](tp) + c0[s](tm) )/2*c0[s](t) + 1 ) )*( (dc[s](tp) + dc[s](tm) )/2*c0[s](t) - ( dc[s](t)/c0[s](t) )*( ( c0[s](tp) + c0[s](tm) )/2*c0[s](t) ) );
        }
    }
    // cout << "\n***********\n***********\n***********\nCheckpoint.\n***********\n***********\n***********\n" << endl;

    
    
    
    // plots ///////////////////////////////////////////////////////////////////
    if(doPlot)
    {
        Plot       p;
        DVec       tAxis;

        tAxis.setLinSpaced(nt,1,nt);
        p << PlotRange(Axis::x, 1, nt);

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
