/*
 * sample-noise-analysis.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
 *
 * LatAnalyze 3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze 3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze 3.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <LatAnalyze/Core/OptParser.hpp>
#include <LatAnalyze/Io/Io.hpp>
#include <LatAnalyze/Core/Math.hpp>
#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/Numerical/GslFFT.hpp>
#include <LatAnalyze/Numerical/MinuitMinimizer.hpp>
#include <LatAnalyze/Statistics/XYSampleData.hpp>

using namespace std;
using namespace Latan;
using namespace Math;

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    OptParser      opt;
    bool           parsed;
    string         filename;

    opt.addOption("" , "help"  , OptParser::OptType::trigger, true,
                  "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or (opt.getArgs().size() != 1) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0];
        cerr << "<options> <sample file>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    filename = opt.getArgs()[0];

    // load data ///////////////////////////////////////////////////////////////
    DMatSample sample;

    cout << "-- load data" << endl;
    sample = Io::load<DMatSample>(filename);

    // compute power spectrum //////////////////////////////////////////////////
    DMat       av, err;
    double     l0;
    Index      nSample = sample.size(), n = sample[central].rows();
    DMatSample noise(nSample), pow(nSample, n, 1);
    CMatSample ftBuf(nSample, n, 1);
    GslFFT     fft(n);

    cout << "-- compute power spectrum" << endl;
    FOR_STAT_ARRAY(sample, s)
    {
        sample[s].conservativeResize(n, 1);
    }
    av  = sample.mean();
    err = sample.variance().cwiseSqrt();
    FOR_STAT_ARRAY(sample, s)
    {
        noise[s] = sample[s] - av;
        ftBuf[s].real() = noise[s];
        ftBuf[s].imag().fill(0.);
        fft(ftBuf[s]);
        pow[s] = ftBuf[s].cwiseAbs2().unaryExpr([](const double x){return 10.*log10(x);});
        //pow[s] = ftBuf[s].cwiseAbs2();
        pow[s].conservativeResize(n/2, 1);
    }
    pow[central] = pow.mean();
    // {
    //     Plot p;
    //     DVec x;

    //     x.setLinSpaced(n/2, 0., n/2 - 1.);
    //     p << LogScale(Axis::x);
    //     p << PlotData(x, pow);
    //     p.display();
    // }
    // l0 = pow.mean()(1);
    // FOR_STAT_ARRAY(sample, s)
    // {
    //     pow[s] = pow[s].unaryExpr([l0](const double x){return x - l0;});
    // }

    // fit decay ///////////////////////////////////////////////////////////////
    DVec            x, init(2);
    DMat            fitErr;
    DMatSample      xs(nSample, n/2, 1);
    DSample         beta(nSample);
    XYSampleData    data(nSample);
    MinuitMinimizer min;
    DoubleModel     lin([](const double *x, const double *p){return x[0]*p[0] + p[1];}, 1, 2);

    cout << "-- fit decay" << endl;
    x.setLinSpaced(n/2, 0., n/2 - 1.);
    FOR_VEC(x, i)
    {
        x(i) = log2(x(i));
    }
    xs.fill(x);
    data.addXDim(n/2, "f", true);
    data.addYDim("pow");
    data.setUnidimData(xs, pow);
    data.assumeYYCorrelated(true, 0, 0);
    for (unsigned int i = 0; i < n/2; ++i)
    {
        data.fitPoint((x(i) >= 2.) and (x(i) <= log2(n/2) - 0.5), i);
    }
    init(0) = -0.1; init(1) = -0.1;
    auto fit = data.fit(min, init, lin);
    fitErr   = fit.variance().cwiseSqrt();
    FOR_STAT_ARRAY(beta, s)
    {
        beta[s] = fit[s](0)/(10.*log10(2.));
    }
    printf("chi^2/dof = %.1e/%d= %.2e -- chi^2 CCDF = %.2e -- p-value = %.2e -- CDR = %.1f dB\n",
        fit.getChi2(), static_cast<int>(fit.getNDof()), fit.getChi2PerDof(), 
        fit.getCcdf(), fit.getPValue(), fit.getCorrRangeDb());
    printf("    decay = %.2f +/- %.2f dB/oct\n", fit[central](0), fitErr(0));
    printf(" exponent = %.2f +/- %.2f\n", beta[central], sqrt(beta.variance()));

    Plot p;

    p << Caption("noise power spectrum");
    p << PlotRange(Axis::x, -0.5, log2(n/2) + 0.5) 
      << Label("frequency (oct)", Axis::x) << Label("power (dB)", Axis::y);
    p << Color("1") << PlotPredBand(fit.getModel(_), 0., log2(n/2) + 0.5);
    p << Color("1") << PlotFunction(fit.getModel(), 0., log2(n/2) + 0.5);
    p << Color("2") << PlotData(x, pow);

    p.display();

    // p.reset();
    // p << PlotCorrMatrix(data.getFitCorrMat());
    // p.display();

    // filter correlator ///////////////////////////////////////////////////////
    DVec       filter(n);
    DMatSample fsample(nSample, n, 1);

    FOR_VEC(filter, i)
    {
        filter(i) = -std::pow(2.*sin(pi/n*i), 2);//-beta[central]*.5);
    }
    FOR_STAT_ARRAY(sample, s)
    {
        ftBuf[s].real() = sample[s].col(0);
        ftBuf[s].imag().fill(0.);
        fft(ftBuf[s], FFT::Forward);
        ftBuf[s] = ftBuf[s].cwiseProduct(filter);
        fft(ftBuf[s], FFT::Backward);
        fsample[s] = ftBuf[s].real();
    }

    // p.reset();
    x.setLinSpaced(n, 0., n - 1.);
    // p << PlotData(x, sample);
    // p << PlotData(x, fsample);
    // p.display();

    p.reset();
    FOR_VEC(x, i)
    {
        x(i) = log2(x(i));
    }
    p << PlotRange(Axis::x, -0.5, log2(n/2) + 0.5);
    p << PlotPoints(x, -filter);
    p.display();
    p.reset();
    p << PlotCorrMatrix(sample.correlationMatrix());
    p.display();
    p.reset();
    p << PlotCorrMatrix(fsample.correlationMatrix());
    p.display();
    Io::save(fsample, "test.h5");


    return EXIT_SUCCESS;
}
