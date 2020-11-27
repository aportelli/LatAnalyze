/*
 * sample-ft.cpp, part of LatAnalyze 3
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
#include <LatAnalyze/Numerical/GslFFT.hpp>
#include <LatAnalyze/Io/Io.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    OptParser    opt;
    bool         parsed;
    string       inFilename, outFilename;
    unsigned int dir = FFT::Forward;
    
    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file name (default: result not saved)", "");
    opt.addOption("b", "backward", OptParser::OptType::trigger, true,
                  "backward Fourier transform (forward by default)");
    opt.addOption("" , "help"  , OptParser::OptType::trigger, true,
                  "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or (opt.getArgs().size() != 1) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0];
        cerr << " <options> <input file>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    inFilename  = opt.getArgs()[0];
    outFilename = opt.optionValue("o");
    if (opt.gotOption("b"))
    {
        dir = FFT::Backward;
    }
    
    // Fourier transform ///////////////////////////////////////////////////////
    DMatSample in          = Io::load<DMatSample>(inFilename);
    Index      nSample     = in.size(), l = in[central].rows();
    bool       isInComplex = (in[central].cols() > 1);
    CMatSample res(nSample, l, 1);
    DMatSample out(nSample, l, 2);
    GslFFT     ft(l);
    
    cout << "-- computing Fourier transform..." << endl;
    FOR_STAT_ARRAY(in, s)
    {
        res[s].real() = in[s].col(0);
        if (isInComplex)
        {
            res[s].imag() = in[s].col(1);
        }
        else
        {
            res[s].imag() = DVec::Constant(l, 0.);
        }
        ft(res[s], dir);
        out[s].col(0) = res[s].real();
        out[s].col(1) = res[s].imag();
    }
    
    // output /////////////////////////////////////////////////////////////////
    cout << scientific;
    cout << "central value:\n"      << out[central];
    cout << endl;
    cout << "standard deviation:\n" << out.variance().cwiseSqrt();
    cout << endl;
    if (!outFilename.empty())
    {
        Io::save<DMatSample>(out, outFilename);
    }
    
    return EXIT_SUCCESS;
}
