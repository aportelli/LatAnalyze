/*
 * sample_plot_corr.cpp, part of LatAnalyze 3
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

using namespace std;
using namespace Latan;
using namespace Math;

int main(int argc, char *argv[])
{
    OptParser      opt;
    bool           parsed;
    string         outCorrName = "", outVarName = "";

    opt.addOption("", "saveCorr", OptParser::OptType::value, true,
                  "save correlation matrix (default: not saved)");
    opt.addOption("", "saveVar", OptParser::OptType::value, true,
                  "save variance matrix (default: not saved)");
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
    if (opt.gotOption("saveCorr"))
    {
        outCorrName = opt.optionValue("saveCorr");
    }
    if (opt.gotOption("saveVar"))
    {
        outVarName = opt.optionValue("saveVar");
    }
    
    string     fileName = opt.getArgs()[0], name;
    DMatSample sample;
    DMat       var, corr;
    Plot       p;
    
    cout << "-- computing variance matrix from '" << fileName << "'..." << endl;
    name   = Io::getFirstName(fileName);
    sample = Io::load<DMatSample>(fileName);
    sample = sample.block(0, 0, sample[central].rows(), 1);
    var    = sample.varianceMatrix();
    corr   = sample.correlationMatrix();

    cout << "dynamic range " << Math::cdr(corr) << " dB" << endl;
    p << PlotCorrMatrix(corr);
    p.display();
    if (!outVarName.empty())
    {
        Io::save(var, outVarName);
    }
    if (!outCorrName.empty())
    {
        Io::save(corr, outCorrName);
    }
    
    return EXIT_SUCCESS;
}
