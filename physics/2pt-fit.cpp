#include <LatAnalyze/Core/Math.hpp>
#include <LatAnalyze/Core/OptParser.hpp>
#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/Functional/CompiledModel.hpp>
#include <LatAnalyze/Io/Io.hpp>
#include <LatAnalyze/Numerical/DWT.hpp>
#include <LatAnalyze/Numerical/MinuitMinimizer.hpp>
#include <LatAnalyze/Numerical/NloptMinimizer.hpp>
#include <LatAnalyze/Physics/CorrelatorFitter.hpp>
#include <LatAnalyze/Physics/EffectiveMass.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>
#include <LatAnalyze/Statistics/XYSampleData.hpp>

using namespace std;
using namespace Latan;

struct TwoPtFit
{
    SampleFitResult result;
    Index           tMin, tMax;
};

int main(int argc, char *argv[])
{
    // parse arguments /////////////////////////////////////////////////////////
    OptParser            opt;
    bool                 parsed, doLaplace, doPlot, doHeatmap, doCorr, fold, doScan, noGuess, doGlobal;
    string               corrFileName, model, outFileName, outFmt, savePlot;
    Index                ti, tf, shift, nPar, thinning;
    double               svdTol;
    Minimizer::Verbosity verbosity;
    
    opt.addOption("" , "ti"       , OptParser::OptType::value  , false,
                  "initial fit time");
    opt.addOption("" , "tf"       , OptParser::OptType::value  , false,
                  "final fit time");
    opt.addOption("t" , "thinning", OptParser::OptType::value  , true,
                  "thinning of the time interval", "1");
    opt.addOption("s", "shift"    , OptParser::OptType::value  , true,
                  "time variable shift", "0");
    opt.addOption("m", "model"    , OptParser::OptType::value  , true,
                  "fit model (exp<n>|sinh<n>|cosh<n>|linear|cst|<interpreter code>)", "exp1");
    opt.addOption("" , "nPar"     , OptParser::OptType::value  , true,
                  "number of model parameters for custom models "
                  "(-1 if irrelevant)", "-1");
    opt.addOption("" , "svd"      , OptParser::OptType::value  , true,
                  "singular value elimination threshold", "0.");
    opt.addOption("v", "verbosity", OptParser::OptType::value  , true,
                  "minimizer verbosity level (0|1|2)", "0");
    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file", "");
    opt.addOption("" , "uncorr"   , OptParser::OptType::trigger, true,
                  "only do the uncorrelated fit");
    opt.addOption("" , "fold"   , OptParser::OptType::trigger, true,
                  "fold the correlator");
    opt.addOption("l" , "laplace"   , OptParser::OptType::trigger, true,
                  "apply Laplace filter to the correlator");
    opt.addOption("" , "subtract"   , OptParser::OptType::value, true,
                  "subtract correlator backwards", "");
    opt.addOption("p", "plot"     , OptParser::OptType::trigger, true,
                  "show the fit plot");
    opt.addOption("h", "heatmap"  , OptParser::OptType::trigger, true,
                  "show the fit correlation heatmap");
    opt.addOption("", "no-guess"  , OptParser::OptType::trigger, true,
                  "do not try to guess fit parameters");
    opt.addOption("", "save-plot", OptParser::OptType::value, true,
                    "saves the source and .pdf", "");
    opt.addOption("", "gevp-to", OptParser::OptType::value, true,
                    "fixed gevp t0", "-1");
    opt.addOption("", "scan", OptParser::OptType::trigger, true,
                    "scan all possible fit ranges within [ti,tf]");
    opt.addOption("", "no-global", OptParser::OptType::trigger, true,
                    "do not use global minimizer");
    opt.addOption("", "help"      , OptParser::OptType::trigger, true,
                  "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or (opt.getArgs().size() != 1) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0] << " <options> <correlator file>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    corrFileName = opt.getArgs().front();
    ti           = opt.optionValue<Index>("ti");
    tf           = opt.optionValue<Index>("tf");
    thinning     = opt.optionValue<Index>("t");
    doLaplace    = opt.gotOption("l");
    shift        = opt.optionValue<Index>("s");
    model        = opt.optionValue("m");
    nPar         = opt.optionValue<Index>("nPar");
    svdTol       = opt.optionValue<double>("svd");
    outFileName  = opt.optionValue<string>("o");
    doCorr       = !opt.gotOption("uncorr");
    fold         = opt.gotOption("fold");
    doPlot       = opt.gotOption("p");
    doHeatmap    = opt.gotOption("h");
    noGuess      = opt.gotOption("no-guess");
    savePlot     = opt.optionValue("save-plot");
    doScan       = opt.gotOption("scan");
    doGlobal     = !opt.gotOption("no-global");

    switch (opt.optionValue<unsigned int>("v"))
    {
        case 0:
            verbosity = Minimizer::Verbosity::Silent;
            break;
        case 1:
            verbosity = Minimizer::Verbosity::Normal;
            break;
        case 2:
            verbosity = Minimizer::Verbosity::Debug;
            break;
        default:
            cerr << "error: wrong verbosity level" << endl;
            return EXIT_FAILURE;
    }
    
    // load correlator /////////////////////////////////////////////////////////
    DMatSample tmp, corr;
    Index      nSample, nt;
    
    corr    = Io::load<DMatSample>(corrFileName);
    nSample = corr.size();
    nt      = corr[central].rows();
    corr    = corr.block(0, 0, nt, 1);
    corr    = CorrelatorUtils::shift(corr, shift);
    if (doLaplace)
    {
        vector<double> filter = {1., -2., 1.};
        DVec           buf;

        FOR_STAT_ARRAY(corr, s)
        {
            DWT::filterConvolution(buf, corr[s], filter, 1);
            corr[s] = buf;
        }
    }
    if (fold)
    {
        corr = CorrelatorUtils::fold(corr);
    }
    if(!opt.optionValue<string>("subtract").empty())
    {
        cout << "Redefining (backwards) C(t) -> C(t-" << opt.optionValue<string>("subtract") << ") - C(t) " << endl;
        Latan::DMatSample tmp = corr;
        for(uint t=opt.optionValue<uint>("subtract") ; t<nt ; t++)
        {
            FOR_STAT_ARRAY(corr,s)
            {
                corr[s](t-opt.optionValue<uint>("subtract"),0) = tmp[s](t-opt.optionValue<uint>("subtract"),0) - tmp[s](t,0);
            }
        }
    }
    
    // make model //////////////////////////////////////////////////////////////
    CorrelatorFitter fitter(corr);
    DoubleModel      mod;
    auto             modelPar = CorrelatorModels::parseModel(model);
    
    if (modelPar.type != CorrelatorType::undefined)
    {
        mod  = CorrelatorModels::makeModel(modelPar, nt, opt.optionValue<int>("gevp-to"));
        nPar = mod.getNPar();
    }
    else
    {
        if (nPar > 0)
        {
            mod = compile(model, 1, nPar);
        }
        else
        {
            cerr << "error: please specify the number of model parameter"
                    " using the --nPar function" << endl;
            
            return EXIT_FAILURE;
        }
    }
    
    // fit /////////////////////////////////////////////////////////////////////
    DVec                init(nPar);
    NloptMinimizer      globMin(NloptMinimizer::Algorithm::GN_CRS2_LM);
    MinuitMinimizer     locMin;
    vector<Minimizer *> unCorrMin;
    if(doGlobal)
    {
        unCorrMin.push_back(&globMin);
    }
    unCorrMin.push_back(&locMin);

    // set fitter **************************************************************
    fitter.setModel(mod);
    fitter.data().setSvdTolerance(svdTol);
    fitter.setThinning(thinning);

    // set initial values ******************************************************
    if ((modelPar.type != CorrelatorType::undefined) and !noGuess)
    {
        init = CorrelatorModels::parameterGuess(corr, modelPar);
    }
    else if(isnan(init.sum()))
    {
        init.fill(1.);
        init(0) = 0.2;
    }

    // set limits for minimisers ***********************************************
    for (Index p = 0; p < nPar; p += 2)
    {
        if ((modelPar.type == CorrelatorType::exp) or
            (modelPar.type == CorrelatorType::cosh) or
            (modelPar.type == CorrelatorType::sinh) or
            (modelPar.type == CorrelatorType::exp_geometric))
        {
            globMin.setLowLimit(p, 0.);
            locMin.setLowLimit(p, 0.);
            globMin.setHighLimit(p, 10.*init(p));
            globMin.setLowLimit(p + 1, -10.*fabs(init(p + 1)));
            globMin.setHighLimit(p + 1, 10.*fabs(init(p + 1)));
        }
        else if(modelPar.type == CorrelatorType::linear)
        {
            globMin.setLowLimit(p, -10.*fabs(init(p)));
            locMin.setLowLimit(p, -10.*fabs(init(p)));
            globMin.setHighLimit(p, 10.*init(p));
            globMin.setLowLimit(p + 1, -10.*fabs(init(p + 1)));
            globMin.setHighLimit(p + 1, 10.*fabs(init(p + 1)));
        }
        else
        {
            globMin.setLowLimit(p, -10*fabs(init(p)));
            globMin.setHighLimit(p, 10*fabs(init(p)));
        }
    }
    if(modelPar.type == CorrelatorType::exp_const)    //odd nPar
    {
        for (Index p = 0; p < nPar-1; p += 2)
        {
            globMin.setLowLimit(p, 0.);
            locMin.setLowLimit(p, 0.);
            globMin.setHighLimit(p, 10.*init(p));
            globMin.setLowLimit(p + 1, -10.*fabs(init(p + 1)));
            globMin.setHighLimit(p + 1, 10.*fabs(init(p + 1)));
        }
        globMin.setLowLimit(nPar-1, 0);
        globMin.setHighLimit(nPar-1, 10.*fabs(init(nPar-1)));
    }
    else if (modelPar.type == CorrelatorType::exp2_complementary) //odd nPar
    {
        for (Index p = 0; p <= 2; p += 2)
        {
            globMin.setLowLimit(p, 0.);
            locMin.setLowLimit(p, 0.);
            globMin.setHighLimit(p, 10.*init(p));
        }
        globMin.setLowLimit(1, -10.*fabs(init(1))); // Z_1
        globMin.setHighLimit(1, 10.*fabs(init(1)));
    }

    globMin.setPrecision(0.001);
    globMin.setMaxIteration(100000);
    globMin.setVerbosity(verbosity);
    locMin.setMaxIteration(1000000);
    locMin.setVerbosity(verbosity);

    // standard fit ////////////////////////////////////////////////////////////
    if (!doScan)
    {
        // fit *****************************************************************
        SampleFitResult fit;

        fitter.setFitRange(ti, tf);
        if (doCorr)
        {
            cout << "-- uncorrelated fit..." << endl;
        }
        cout << "using model '" << model << "'" << endl;
        fitter.setCorrelation(false);
        fit = fitter.fit(unCorrMin, init);
        fit.print();
        if (doCorr)
        {
            cout << "-- correlated fit..." << endl;
            cout << "using model '" << model << "'" << endl;
            init = fit[central];
            fitter.setCorrelation(true);
            fit = fitter.fit(locMin, init);
            fit.print();
        }
        if (!outFileName.empty())
        {
            Io::save(fit, outFileName);
        }
        // plots ***************************************************************
        if (doPlot)
        {
            DMatSample tvec(nSample);

            tvec.fill(DVec::LinSpaced(nt, 0, nt - 1));
            if (modelPar.type != CorrelatorType::cst)
            {
                Plot p;

                p << PlotRange(Axis::x, 0, nt - 1);
                if ((modelPar.type == CorrelatorType::exp) or
                    (modelPar.type == CorrelatorType::cosh) or
                    (modelPar.type == CorrelatorType::sinh))
                {
                    p << LogScale(Axis::y);
                }
                p << Color("rgb 'blue'") << PlotPredBand(fit.getModel(_), 0, nt - 1);
                p << Color("rgb 'blue'") << PlotFunction(fit.getModel(), 0, nt - 1);
                p << Color("rgb 'red'")  << PlotData(fitter.data().getData());
                p << Label("t/a", Axis::x) << Caption("Correlator");
                p.display();
                if(savePlot != "")
                {
                    p.save(savePlot + "_corr");
                }
            }
            if (modelPar.type != CorrelatorType::undefined)
            {
                Plot          p;
                EffectiveMass effMass(modelPar.type);
                DMatSample    em;
                DVec          fitErr, emtvec;
                double        e0, e0Err;

                emtvec = effMass.getTime(nt);
                em     = effMass(corr);
                fitErr = fit.variance().cwiseSqrt();
                e0     = fit[central](0);
                e0Err  = fitErr(0);
                p.reset();
                p << PlotRange(Axis::x, 0, nt - 1);
                p << PlotRange(Axis::y, e0 - 30.*e0Err, e0 + 30.*e0Err);

                if(model=="exp2" or model=="cosh2")
                {
                    Latan::DoubleFunction twoStateEffmass([&fit](const double *x)
                    {
                        const double t = x[0];
                        const Latan::Index s = (int)x[1];
                        const double E0= fit[s](0);
                        const double Z0= fit[s](1);
                        const double E1= fit[s](2);
                        const double Z1= fit[s](3);

                        return ( E0 + (Z1/Z0) * E1 * exp(-(E1-E0) * t) ) / (1 + (Z1/Z0) * exp(-(E1-E0) * t) );
                    },2);
                    auto twoStateEffmassBand = [&twoStateEffmass](const Latan::Index nSample, const Latan::DVec& tfine)
                    {
                        Latan::DMatSample res(nSample, tfine.size(), 1);
                        FOR_STAT_ARRAY(res, s)
                        {
                            FOR_VEC(tfine, t)
                            {
                                res[s](t,0) = twoStateEffmass(t, s);
                            }
                        }
                        return res;
                    };
                    // Latan::DVec tfine = Latan::DVec::LinSpaced(1000, 0, nt - 1);
                    Latan::DMatSample twoStateModel = twoStateEffmassBand(fit.size(), emtvec);
                    p << Color("rgb 'blue'") << PlotPredBand(emtvec, twoStateModel[Latan::central], twoStateModel.variance().cwiseSqrt());
                    p << Color("rgb 'blue'") << PlotFunction(twoStateEffmass.bind(1, Latan::central), 0, nt - 1);
                }
                else
                {
                    p << Color("rgb 'blue'") << PlotBand(0, nt - 1, e0 - e0Err, e0 + e0Err);
                    p << Color("rgb 'blue'") << PlotHLine(e0);
                }

                p << Color("rgb 'red'") << PlotData(emtvec, em);
                p << Label("t/a", Axis::x) << Caption("Effective Mass");
                p.display();
                if(savePlot != "")
                {
                    p.save(savePlot + "_effMass");
                }
            }
            if (doHeatmap)
            {
                Plot  p;
                Index n  = fitter.data().getFitVarMat().rows();
                DMat  id = DMat::Identity(n, n),
                      var = fitter.data().getFitVarMat();
                
                p << PlotCorrMatrix(Math::varToCorr(var));
                p << Caption("correlation matrix");
                p.display();
                if (svdTol > 0.)
                {
                    DMat proj = id - var*fitter.data().getFitVarMatPInv();

                    p.reset();
                    p << PlotMatrix(proj);
                    p << Caption("singular space projector");
                    p.display();
                }
            }
        }

    }
    // scan fits ///////////////////////////////////////////////////////////////
    else
    {
        // fits ****************************************************************
        Index nFit = 0, f = 0, ti0 = ti + (tf - ti)/4, tf0 = tf - (tf - ti)/4,
              matSize = tf - ti + 1;
        DMat  err, pVal(matSize, matSize), relErr(matSize, matSize),
              ccdf(matSize, matSize), val(matSize, matSize);
        map<double, TwoPtFit> fit;
        SampleFitResult       tmpFit;

        cout << "-- initial uncorrelated fit on [" << ti0 << ", " << tf0 << "]..." << endl;
        if (thinning != 1)
        {
            cerr << "warning: thinning different from 1 ignored in scan mode"
                 << endl;
            thinning = 1;
        }
        fitter.setFitRange(ti0, tf0);
        fitter.setCorrelation(false);
        tmpFit = fitter.fit(unCorrMin, init);
        tmpFit.print();
        cout << "-- scanning all possible fit ranges..." << endl;
        init = tmpFit[central];
        fitter.setCorrelation(doCorr);
        pVal.fill(Math::nan);
        relErr.fill(Math::nan);
        val.fill(Math::nan);
        ccdf.fill(Math::nan);
        for (Index ta = ti; ta < tf; ++ta)
        for (Index tb = ta + nPar; tb < tf; ++tb)
        {
            nFit++;
        }
        for (Index ta = ti; ta < tf; ++ta)
        for (Index tb = ta + nPar; tb < tf; ++tb)
        {
            Index  i = ta - ti, j = tb - ti;

            fitter.setFitRange(ta, tb);
            tmpFit              = fitter.fit(locMin, init);
            err                 = tmpFit.variance().cwiseSqrt();
            pVal(i, j)          = tmpFit.getPValue();
            ccdf(i, j)          = tmpFit.getCcdf();
            val(i, j)           = tmpFit[central](0);
            relErr(i, j)        = err(0)/fabs(val(i, j));
            fit[pVal(i, j)].result = tmpFit;
            fit[pVal(i, j)].tMin   = ta;
            fit[pVal(i, j)].tMax   = tb;
            f++;
            cout << "\r[" << ta << ", " << tb << "] "<< ProgressBar(f, nFit);
        }
        cout << endl << endl;
        cout << "TOP 10 fits" << endl;
        cout << "-----------" << endl;
        auto it = fit.rbegin();
        unsigned int k = 0;
        while (k < 10)
        {
            auto &f = it->second;

            cout << "#" << k + 1 << " -- [" << f.tMin << ", " << f.tMax << "] -- ";
            f.result.print();
            cout << endl;
            k++;
            it++;
        }
        // plots ***************************************************************
        if (doPlot)
        {
            Plot p;

            p << PlotMatrix(pVal);
            p << Caption("p-value matrix");
            p << Label("tMax - " + strFrom(ti), Axis::x);
            p << Label("tMin - " + strFrom(ti), Axis::y);
            p.display();
            if(savePlot != "")
            {
                p.save(savePlot + "_pValMatrix");
            }
            p.reset();
            p << PlotMatrix(relErr);
            p << Caption("Relative error matrix");
            p << Label("tMax - " + strFrom(ti), Axis::x);
            p << Label("tMin - " + strFrom(ti), Axis::y);
            p.display();
            if(savePlot != "")
            {
                p.save(savePlot + "_relErrMatrix");
            }
            p.reset();
            p << PlotMatrix(val);
            p << Caption("Fit result matrix");
            p << Label("tMax - " + strFrom(ti), Axis::x);
            p << Label("tMin - " + strFrom(ti), Axis::y);
            p.display();
            if(savePlot != "")
            {
                p.save(savePlot + "_valMatrix");
            }
            p.reset();
            p << PlotMatrix(ccdf);
            p << Caption("chi^2 CCDF matrix");
            p << Label("tMax - " + strFrom(ti), Axis::x);
            p << Label("tMin - " + strFrom(ti), Axis::y);
            p.display();
            if(savePlot != "")
            {
                p.save(savePlot + "_ccdfMatrix");
            }
        }
    }

    return EXIT_SUCCESS;
}
