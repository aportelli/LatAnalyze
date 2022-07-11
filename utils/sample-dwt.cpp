/*
 * sample-dwt.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli, Matt Spraggs
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
#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/Io/Io.hpp>
#include <LatAnalyze/Numerical/DWT.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    OptParser    opt;
    bool         parsed, doPlot, ss, saveAll;
    unsigned int level;
    string       inFilename, outFilename, filterName;
    
    opt.addOption("l", "level", OptParser::OptType::value, true,
                  "number of levels", "1");
    opt.addOption("f", "filter", OptParser::OptType::value, true,
                  "filter name", "haar");
    opt.addOption("s", "ss", OptParser::OptType::trigger, true,
                  "super-sampling (inverse DWT on data)");
    opt.addOption("a", "all", OptParser::OptType::trigger, true,
                  "save all-levels");
    opt.addOption("o", "output", OptParser::OptType::value, true,
                  "output file name, or directory name if --all is used (default: result not saved)", "");
    opt.addOption("p", "plot", OptParser::OptType::trigger, true,
                  "show plot");
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
    level       = opt.optionValue<unsigned int>("l");
    filterName  = opt.optionValue("f");
    ss          = opt.gotOption("s");
    saveAll     = opt.gotOption("a");
    outFilename = opt.optionValue("o");
    doPlot      = opt.gotOption("p");

    // DWT /////////////////////////////////////////////////////////////////////
    DMatSample            in = Io::load<DMatSample>(inFilename), res;
    Index                 nSample = in.size(), n = in[central].rows();
    vector<DMatSample>    out(ss ? 1 : level, DMatSample(nSample)), 
                          outh(ss ? 0 : level, DMatSample(nSample));
    DWT                   dwt(*DWTFilters::fromName.at(filterName));
    vector<DWT::DWTLevel> dataDWT(level);

    if (!ss)
    {
        cout << "-- compute discrete wavelet transform" << endl;
        cout << "filter '" << filterName << "' / " << level << " level(s)" << endl;
        FOR_STAT_ARRAY(in, s)
        {
            dataDWT = dwt.forward(in[s].col(0), level);
            for (unsigned int l = 0; l < level; ++l)
            {
                out[l][s]  = dataDWT[l].first;
                outh[l][s] = dataDWT[l].second;
            }
        }
    }
    else
    {
        Index ssn = n;

        cout << "-- compute inverse discrete wavelet transform" << endl;
        cout << "filter '" << filterName << "' / " << level << " level(s)" << endl;
        for (int l = level - 1; l >= 0; --l)
        {
            dataDWT[l].first.resize(ssn);
            dataDWT[l].second.resize(ssn);
            dataDWT[l].first.fill(0.);
            dataDWT[l].second.fill(0.);
            ssn *= 2;
        }
        FOR_STAT_ARRAY(in, s)
        {
            dataDWT.back().first = in[s].col(0);
            out[0][s] = dwt.backward(dataDWT);
        }
    }
    if (!outFilename.empty())
    {
        if (!ss and saveAll)
        {
            Latan::mkdir(outFilename);
            for (unsigned int l = 0; l < level; ++l)
            {
                Io::save<DMatSample>(out[l], outFilename + "/L" + strFrom(l) + ".h5");
                Io::save<DMatSample>(outh[l], outFilename + "/H" + strFrom(l) + ".h5");
            }
        }
        else
        {
            Io::save<DMatSample>(out.back(), outFilename);
        }
    }

    // plot ////////////////////////////////////////////////////////////////////
    if (doPlot)
    {
        Plot p;
        DVec x;

        x.setLinSpaced(n, 0., n - 1.);
        p << PlotRange(Axis::x, 0., n);
        p << Title("original") << PlotData(x, in);
        if (!ss)
        {
            Index  ln = n, step = 1;

            for (unsigned int l = 0; l < level; ++l)
            {
                ln   /= 2;
                step *= 2;
                x.setLinSpaced(ln, 0., n - step);
                p << Title("level " + strFrom(l + 1) + " L") << PlotData(x, out[l]);
                p << Title("level " + strFrom(l + 1) + " H") << PlotData(x, outh[l]);
            }
            p.display();
        }
        else
        {
            double step = 1.;
            DVec   err;

            for (unsigned int l = 0; l < level; ++l)
            {
                step /= 2.;
            }
            x.setLinSpaced(out[0][central].size(), 0., n - step);
            err = out[0].variance().cwiseSqrt();
            p << Title("supersampled") << Color("3") << PlotPredBand(x, out[0][central], err);
            p << Color("3") << PlotLine(x, out[0][central]);
            p.display();
        }
    }

    return EXIT_SUCCESS;
}
