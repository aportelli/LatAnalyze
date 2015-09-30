/*
 * make_fake_sample.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli
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

#include <iostream>
#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/RandGen.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    Index  nSample;
    double val, err;
    string outFileName;

    if (argc != 5)
    {
        cerr << "usage: " << argv[0];
        cerr << " <central value> <error> <nSample> <output file>" << endl;

        return EXIT_FAILURE;
    }
    val         = strTo<double>(argv[1]);
    err         = strTo<double>(argv[2]);
    nSample     = strTo<Index>(argv[3]);
    outFileName = argv[4];

    RandGen    gen;
    DMatSample res(nSample, 1, 1);

    FOR_STAT_ARRAY(res, s)
    {
        if (s == central)
        {
            res[s](0, 0) = val;
        }
        else
        {
            res[s](0, 0) = gen.gaussian(val, err);
        }
    }
    Io::save<DMatSample>(res, outFileName);

    return EXIT_SUCCESS;
}
