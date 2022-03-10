/*
 * sample-merge.cpp, part of LatAnalyze 3
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

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    OptParser      opt;
    bool           parsed;
    string         outFileName = "";
    vector<string> fileName;
    unsigned int   n = 0;

    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file name (default: result not saved)");
    opt.addOption("" , "help"  , OptParser::OptType::trigger, true,
                  "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or (opt.getArgs().size() < 1) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0];
        cerr << " <sample 1> ... <sample n>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    n           = opt.getArgs().size();
    outFileName = opt.optionValue("o");
    for (unsigned int i = 0; i < n; ++i)
    {
        fileName.push_back(opt.getArgs()[i]);
    }

    // figure out dimensions ///////////////////////////////////////////////////
    Index      nCol, nSample, totRows;
    DMatSample buf;

    buf     = Io::load<DMatSample>(fileName[0]);
    nSample = buf.size();
    totRows = buf[central].rows();
    nCol    = buf[central].cols();
    for (unsigned int i = 1; i < n; ++i)
    {
        buf = Io::load<DMatSample>(fileName[i]);
        if (buf.size() != nSample)
        {
            LATAN_ERROR(Size, "sample size mismatch");
        }
        if (buf[central].cols() != nCol)
        {
            LATAN_ERROR(Size, "column size mismatch");
        }
        totRows += buf[central].rows();
    }

    // merge along rows ////////////////////////////////////////////////////////
    DMatSample out(nSample, totRows, nCol);
    Index      rowo = 0, r;

    for (unsigned int i = 0; i < n; ++i)
    {
        buf = Io::load<DMatSample>(fileName[i]);
        r   = buf[central].rows();
        out.block(rowo, 0, r, nCol) = buf;
        rowo += r;
    }
    if (!outFileName.empty())
    {
        Io::save<DMatSample>(out, outFileName);
    }

    return EXIT_SUCCESS;
}