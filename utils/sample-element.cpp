/*
 * sample-element.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli, Matt Spraggs
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

#include <LatCore/OptParser.hpp>
#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/MatSample.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    OptParser opt;
    bool      parsed;
    string    inFilename, outFilename;
    Index     r, c, nr, nc;
    
    opt.addOption("r", "row",    OptParser::OptType::value  , false,
                  "row");
    opt.addOption("c", "col",    OptParser::OptType::value  , false,
                  "column");
    opt.addOption("" , "nrow",   OptParser::OptType::value  , true,
                  "number of rows (default: 1)", "1");
    opt.addOption("" , "ncol",   OptParser::OptType::value  , true,
                  "number of columns (default: 1)", "1");
    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file name (default: result not saved)", "");
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
    r           = opt.optionValue<Index>("r");
    c           = opt.optionValue<Index>("c");
    nr          = opt.optionValue<Index>("nrow");
    nc          = opt.optionValue<Index>("ncol");
    
    // Data extraction /////////////////////////////////////////////////////////
    auto inputData = Io::load<DMatSample>(inFilename);
    
    if ((nr == 1) and (nc == 1))
    {
        DSample outputData(inputData.size());
        
        FOR_STAT_ARRAY(inputData, s)
        {
            outputData[s] = inputData[s](r, c);
        }
        
        cout << scientific;
        cout << "central value:\n" << outputData[central] << endl;
        cout << "standard deviation:\n";
        cout << sqrt(outputData.variance()) << endl;
        if (not outFilename.empty())
        {
            Io::save(outputData, outFilename);
        }
    }
    else
    {
        DMatSample outputData(inputData.size(), nr, nc);
        
        FOR_STAT_ARRAY(inputData, s)
        {
            outputData[s] = inputData[s].block(r, c, nr, nc);
        }
        cout << scientific;
        cout << "central value:\n" << outputData[central] << endl;
        cout << "standard deviation:\n";
        cout << outputData.variance().cwiseSqrt() << endl;
        if (not outFilename.empty())
        {
            Io::save(outputData, outFilename);
        }
    }

    return EXIT_SUCCESS;
}
