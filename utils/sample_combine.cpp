/*
 * sample_combine.cpp, part of LatAnalyze 3
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
#include <libgen.h>
#include <unistd.h>
#include <LatAnalyze/AsciiFile.hpp>
#include <LatAnalyze/CompiledFunction.hpp>

using namespace std;
using namespace Latan;

static void usage(const string &cmdName)
{
    cerr << "usage: " << cmdName;
    cerr << " [-o <output sample>]";
    cerr << " <n> <function> <sample 1> ... <sample n>";
    cerr << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    string         cmdName, outFileName = "", code;
    vector<string> fileName;
    int            c;
    unsigned int   n = 0;

    opterr = 0;
    cmdName = basename(argv[0]);
    while ((c = getopt(argc, argv, "o:")) != -1)
    {
        switch (c)
        {
            case 'o':
                outFileName = optarg;
                break;
            case '?':
                cerr << "error parsing option -" << char(optopt) << endl;
                usage(cmdName);
                break;
            default:
                usage(cmdName);
                break;
        }
    }
    if (argc - optind >= 1)
    {
        n = strTo<unsigned int>(argv[optind]);
        if (argc - optind == static_cast<int>(n + 2))
        {
            fileName.resize(n);
            code = argv[optind + 1];
            for (unsigned int i = 0; i < n; ++i)
            {
                fileName[i] = argv[optind + 2 + i];
            }
        }
        else
        {
            usage(cmdName);
        }
    }
    else
    {
        usage(cmdName);
    }

    // data loading ////////////////////////////////////////////////////////////
    vector<DMatSample> sample(n);
    Index              nSample;

    cout << "-- loading data..." << endl;
    for (unsigned int i = 0; i < n; ++i)
    {
        sample[i] = Io::load<DMatSample, AsciiFile>(fileName[i]);
        if (i == 0)
        {
            nSample = sample[i].size();
        }
        else if (sample[i].size() != nSample)
        {
            cerr << "error: number of sample mismatch (between '";
            cerr << fileName[0] << "' and '" << fileName[i] << "')" << endl;

            return EXIT_FAILURE;
        }
    }

    // combine data ////////////////////////////////////////////////////////////
    DoubleFunction f = compile(code, n);
    DVec           buf(n);
    DMatSample     result(sample[0]);

    cout << "-- combining data..." << endl;
    FOR_STAT_ARRAY(result, s)
    {
        FOR_MAT(result[s], i, j)
        {
            for (unsigned int k = 0; k < n; ++k)
            {
                buf[k] = sample[k][s](i,j);
            }
            result[s](i, j) = f(buf);
        }
    }

    // output //////////////////////////////////////////////////////////////////
    cout << scientific;
    cout << "central value:\n"      << result[central]               << endl;
    cout << "standard deviation:\n" << result.variance().cwiseSqrt() << endl;
    if (!outFileName.empty())
    {
        Io::save<DMatSample, AsciiFile>(result, outFileName);
    }

    return EXIT_SUCCESS;
}
