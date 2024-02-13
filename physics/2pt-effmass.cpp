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

int main(int argc, char *argv[])
{
    // parse arguments /////////////////////////////////////////////////////////
    OptParser            opt;
    bool                 parsed, fold;
    string               corrFileName, model, outFileName,savePlot;
    Index                nPar, shift;

    opt.addOption("m", "model"    , OptParser::OptType::value  , true,
                  "fit model (exp<n>|sinh<n>|cosh<n>|linear|cst|<interpreter code>)", "exp1");
    opt.addOption("" , "nPar"     , OptParser::OptType::value  , true,
                  "number of model parameters for custom models "
                  "(-1 if irrelevant)", "-1");
    opt.addOption("" , "fold"   , OptParser::OptType::trigger, true,
                  "fold the correlator");
    opt.addOption("s", "shift"    , OptParser::OptType::value  , true,
                  "time variable shift", "0");
    opt.addOption("", "save-plot", OptParser::OptType::trigger, true,
                    "saves the source and .pdf", "");
    opt.addOption("p", "show-plot" , OptParser::OptType::trigger, true, "display gnuplot plots");
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
    shift        = opt.optionValue<Index>("s");
    model        = opt.optionValue("m");
    nPar         = opt.optionValue<Index>("nPar");
    fold         = opt.gotOption("fold");
    savePlot     = corrFileName.substr(0, corrFileName.size()-3); //remove .h5 

    
    // load correlator /////////////////////////////////////////////////////////
    DMatSample tmp, corr;
    Index      nSample, nt;
    
    corr    = Io::load<DMatSample>(corrFileName);
    nSample = corr.size();
    nt      = corr[central].rows();
    corr    = corr.block(0, 0, nt, 1);
    corr    = CorrelatorUtils::shift(corr, shift);

    if (fold)
    {
        corr = CorrelatorUtils::fold(corr);
    }
    
    // make model //////////////////////////////////////////////////////////////
    CorrelatorFitter fitter(corr);
    DoubleModel      mod;
    auto             modelPar = CorrelatorModels::parseModel(model);
    
    if (modelPar.type != CorrelatorType::undefined)
    {
        mod  = CorrelatorModels::makeModel(modelPar, nt);
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
    
    // plots ***************************************************************
    
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
        p << Color("rgb 'red'")  << PlotData(tvec,corr);
        p << Label("t/a", Axis::x) << Caption("Correlator");
        if(opt.gotOption("show-plot"))
            p.display();
        if(opt.gotOption("save-plot"))
            p.save(savePlot + "_corr");
    }
    if (modelPar.type != CorrelatorType::undefined)
    {
        Plot          p;
        EffectiveMass effMass(modelPar.type);
        DMatSample    em;
        DVec          emtvec;

        emtvec = effMass.getTime(nt);
        em     = effMass(corr);
        p.reset();
        p << PlotRange(Axis::x, 0, nt - 1);
        p << Color("rgb 'red'") << PlotData(emtvec, em);
        p << Label("t/a", Axis::x) << Caption("Effective Mass");
        if(opt.gotOption("show-plot"))
            p.display();
        if(opt.gotOption("save-plot"))
        {
            p.save(savePlot + "_effMass");
            cout << "Saving plots to stem " << savePlot << endl;
        }
    }

    return EXIT_SUCCESS;
}
