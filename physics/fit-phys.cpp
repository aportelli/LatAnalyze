#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/MinuitMinimizer.hpp>
#include <LatAnalyze/NloptMinimizer.hpp>
#include <LatAnalyze/Plot.hpp>
#include "fit-phys-env.hpp"

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    // parse arguments /////////////////////////////////////////////////////////
    string paramFileName;
    
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <parameter file>" << endl;
        
        return EXIT_FAILURE;
    }
    paramFileName = argv[1];
    
    // parse XML & load data ///////////////////////////////////////////////////
    FitEnv env;
    
    env.parseXml(paramFileName);
    env.load();
    
    XYSampleData uncorrData = env.generateData(false, false);
    XYSampleData corrData   = env.generateData(false, true);
    
    cout << "DATA SUMMARY" << endl;
    cout << "============" << endl;
    cout << env << uncorrData << endl;
    
    // fit /////////////////////////////////////////////////////////////////////
    auto v = env.getModels();
    SampleFitResult fit;
    MinuitMinimizer min1, min2;
    vector<Minimizer *> min{&min1, &min2};
    DVec            init(v[0]->getNPar());
    
    min1.setVerbosity(Minimizer::Verbosity::Normal);
    min2.setVerbosity(Minimizer::Verbosity::Normal);
    min1.setMaxIteration(1000000);
    min1.setPrecision(1.0e-3);
    min2.setMaxIteration(1000000);
    min2.setPrecision(1.0e-5);
    init.fill(1.0);
    fit = uncorrData.fit(min, init, v);
    fit.print();
    init = fit[central].block(0, 0, init.size(), 1);
    fit = corrData.fit(min2, init, v);
    fit.print();
    
//    init = fit[central].block(0, 0, v[0]->getNPar(), 1);
//    min1.setVerbosity(Minimizer::Verbosity::Normal);
//    fit = corrData.fit(min1, init, v);
    
    // plot ////////////////////////////////////////////////////////////////////
//    Plot p;
//    DVec phyPt = env.getPhyPt();
//    phyPt(env.getVarIndex("a")) = 1.;
//    XYSampleData projData = uncorrData.getPartialResiduals(fit, phyPt, env.getVarIndex("M_Ds"));
//    
//    p << PlotPredBand(fit.getModel(_).bind(env.getVarIndex("M_Ds"), phyPt), 0., 3.);
//    p << PlotData(projData.getData(), env.getVarIndex("M_Ds"), 0);
//    p.display();
//    p.reset();
//    projData = uncorrData.getPartialResiduals(fit, phyPt, env.getVarIndex("a"));
//    p << PlotPredBand(fit.getModel(_).bind(env.getVarIndex("a"), phyPt), 0., 1.);
//    p << PlotData(projData.getData(), env.getVarIndex("a"), 0);
//    p.display();
//    p.reset();
    
    return EXIT_SUCCESS;
}
