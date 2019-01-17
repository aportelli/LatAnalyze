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
        cerr << "usage: " << argv[0] << " <options> <correlator file 1> <correlator file 2>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    corrFileName = opt.getArgs()[0];
    corr0FileName = opt.getArgs()[1];
    outFileName  = opt.optionValue<string>("o");
    shift        = opt.optionValue<Index>("s");
    doPlot       = opt.gotOption("p");
    
    // load correlator /////////////////////////////////////////////////////////
    DMatSample tmp, corr0, dcorr, effmass;
    Index      nSample, nt;
    float      tp,tm;
    
    tmp     = Io::load<DMatSample>(corr0FileName);
    nSample = tmp.size();
    nt      = tmp[central].rows();
    tmp     = tmp.block(0, 0, nt, 1);
    corr0   = tmp;
    dcorr   = tmp;
    effmass = tmp; // initialise effmass like this
    FOR_STAT_ARRAY(corr0, s) // loads the QCD correlator, bootstrap sample by sample
    {
        for (Index t = 0; t < nt; ++t) 
        {
            corr0[s]((t - shift + nt)%nt) = tmp[s](t);
        }
    }
    tmp     = Io::load<DMatSample>(corrFileName);
    tmp     = tmp.block(0, 0, nt, 1);
    FOR_STAT_ARRAY(dcorr, s) // computes the leading order perturbation in corr
    {
        for (Index t = 0; t < nt; ++t) 
        {
            dcorr[s](t) = tmp[s](t) - corr0[s](t);
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

            effmass[s](t) = (1./sqrt( ( corr0[s](tp) + corr0[s](tm) )/2*corr0[s](t) - 1 ))*( (dcorr[s](tp) + dcorr[s](tm) )/2*corr0[s](t) - ( dcorr[s](t)/corr0[s](t) )*( ( corr0[s](tp) + corr0[s](tm) )/corr0[s](t) ) );
        }
    }
    // cout << "\n***********\n***********\n***********\nCheckpoint.\n***********\n***********\n***********\n" << endl;

    
    
    
    // plots ///////////////////////////////////////////////////////////////////
    if(doPlot)
    {
        Plot       p;
        DVec       tAxis;

        int ymax = effmass[central](nt/2);
        tAxis.setLinSpaced(nt,1,nt);
        p << PlotRange(Axis::x, 0, nt - 1);
        p << PlotRange(Axis::y, 0, ymax);

        p << Color("rgb 'red'") << PlotData(tAxis, effmass);
        p.display();

    }
    /*if (doPlot)
    {
        Plot       p;
        DMatSample effMass(nSample);
        DVec       effMassT, fitErr;
        Index      maxT = (coshModel) ? (nt - 2) : (nt - 1);
        double     e0, e0Err;
        
        p << PlotRange(Axis::x, 0, nt - 1);
        if (!linearModel)
        {
            p << LogScale(Axis::y);
        }
        p << Color("rgb 'blue'") << PlotPredBand(fit.getModel(_), 0, nt - 1);
        p << Color("rgb 'blue'") << PlotFunction(fit.getModel(), 0, nt - 1);
        p << Color("rgb 'red'") << PlotData(data.getData());
        p.display();
        effMass.resizeMat(maxT, 1);
        effMassT.setLinSpaced(maxT, 1, maxT);
        fitErr = fit.variance().cwiseSqrt();
        e0     = fit[central](0);
        e0Err  = fitErr(0);
        if (coshModel)
        {
            FOR_STAT_ARRAY(effMass, s)
            {
                for (Index t = 1; t < nt - 1; ++t)
                {
                    effMass[s](t - 1) = acosh((corr[s](t-1) + corr[s](t+1))
                                              /(2.*corr[s](t)));
                }
            }
        }
        else if (linearModel)
        {
            FOR_STAT_ARRAY(effMass, s)
            {
                for (Index t = 0; t < nt - 1; ++t)
                {
                    effMass[s](t) = corr[s](t) - corr[s](t+1);
                }
            }
        }
        else
        {
            FOR_STAT_ARRAY(effMass, s)
            {
                for (Index t = 1; t < nt; ++t)
                {
                    effMass[s](t - 1) = log(corr[s](t-1)/corr[s](t));
                }
            }
        }
        p.reset();
        p << PlotRange(Axis::x, 1, maxT);
        p << PlotRange(Axis::y, e0 - 20.*e0Err, e0 + 20.*e0Err);
        p << Color("rgb 'blue'") << PlotBand(0, maxT, e0 - e0Err, e0 + e0Err);
        p << Color("rgb 'blue'") << PlotHLine(e0);
        p << Color("rgb 'red'") << PlotData(effMassT, effMass);
        p.display();
    }
    if (doHeatmap)
    {
        Plot  p;
        Index n  = data.getFitVarMat().rows();
        DMat  id = DMat::Identity(n, n);
        
        p << PlotMatrix(Math::varToCorr(data.getFitVarMat()));
        p << Caption("correlation matrix");
        p.display();
        if (svdTol > 0.)
        {
            p.reset();
            p << PlotMatrix(id - data.getFitVarMat()*data.getFitVarMatPInv());
            p << Caption("singular space projector");
            p.display();
        }
    }
    
    // output //////////////////////////////////////////////////////////////////
    if (!outFileName.empty())
    {
        Io::save(fit, outFileName);
    }

    return EXIT_SUCCESS;*/
}
