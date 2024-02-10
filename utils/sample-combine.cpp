/*
 * sample_combine.cpp, part of LatAnalyze 3
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
#include <LatAnalyze/Functional/CompiledFunction.hpp>

using namespace std;
using namespace Latan;

template <typename T>
static void loadAndCheck(vector<T> &sample __dumb,
                         const vector<string> &fileName __dumb)
{
    abort();
}

template <>
void loadAndCheck(vector<DSample> &sample, const vector<string> &fileName)
{
    const unsigned int n = static_cast<unsigned int>(sample.size());
    Index              nSample = 0;
    
    for (unsigned int i = 0; i < n; ++i)
    {
        sample[i] = Io::load<DSample>(fileName[i]);
        if (i == 0)
        {
            nSample = sample[i].size();
        }
        else if (sample[i].size() != nSample)
        {
            cerr << "error: number of sample mismatch (between '";
            cerr << fileName[0] << "' and '" << fileName[i] << "')" << endl;
            abort();
        }
    }
}

template <>
void loadAndCheck(vector<DMatSample> &sample, const vector<string> &fileName)
{
    const unsigned int n = static_cast<unsigned int>(sample.size());
    Index              nSample = 0;
    set<unsigned int>  failed;
    bool               gotSize = false;
    Index              nRow = 0, nCol = 0;
    
    for (unsigned int i = 0; i < n; ++i)
    {
        try
        {
            sample[i] = Io::load<DMatSample>(fileName[i]);
            if (!gotSize)
            {
                nSample = sample[i].size();
                nRow    = sample[i][central].rows();
                nCol    = sample[i][central].cols();
                gotSize = true;
            }
        }
        catch (Exceptions::Definition &)
        {
            failed.insert(i);
        }
    }
    for (unsigned int i: failed)
    {
        DSample buf;
        
        buf = Io::load<DSample>(fileName[i]);
        sample[i].resize(nSample);
        FOR_STAT_ARRAY(sample[i], s)
        {
            sample[i][s] = DMat::Constant(nRow, nCol, buf[s]);
        }
    }
    for (unsigned int i = 0; i < n; ++i)
    {
        if (sample[i].size() != nSample)
        {
            cerr << "error: number of sample mismatch (between '";
            cerr << fileName[0] << "' and '" << fileName[i] << "')" << endl;
            abort();
        }
        if ((sample[i][central].rows() != nRow) and
            (sample[i][central].cols() != nCol))
        {
            cerr << "error: matrix size mismatch (between '";
            cerr << fileName[0] << "' and '" << fileName[i] << "')" << endl;
            abort();
        }
    }
}

template <typename T>
static void combine(const string &outFileName __dumb,
                    const vector<T> &sample __dumb, const string &code __dumb)
{
    abort();
}

template <>
void combine(const string &outFileName, const vector<DSample> &sample,
             const string &code)
{
    const unsigned int n = static_cast<unsigned int>(sample.size());
    DoubleFunction     f = compile(code, n);
    DSample            result(sample[0]);
    DVec               buf(n);
    
    cout << "-- combining data..." << endl;
    result = sample[0];
    FOR_STAT_ARRAY(result, s)
    {
        for (unsigned int k = 0; k < n; ++k)
        {
            buf[k] = sample[k][s];
        }
        result[s] = f(buf);
    }
    cout << scientific;
    cout << "central value:\n"      << result[central];
    cout << endl;
    cout << "standard deviation:\n" << sqrt(result.variance());
    cout << endl;
    if (!outFileName.empty())
    {
        Io::save<DSample>(result, outFileName);
    }
}

template <>
void combine(const string &outFileName, const vector<DMatSample> &sample,
             const string &code)
{
    const unsigned int n = static_cast<unsigned int>(sample.size());
    DoubleFunction     f = compile(code, n);
    DVec               buf(n);
    DMatSample         result(sample[0]);
    
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
    cout << scientific;
    cout << "central value:\n"      << result[central];
    cout << endl;
    cout << "standard deviation:\n" << result.variance().cwiseSqrt();
    cout << endl;
    if (!outFileName.empty())
    {
        Io::save<DMatSample>(result, outFileName);
    }
}

template <typename T>
void process(const string &outFileName, const vector<string> &fileName,
             const string &code)
{
    vector<T> sample(fileName.size());
    
    loadAndCheck(sample, fileName);
    combine(outFileName, sample, code);
}

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    OptParser      opt;
    bool           parsed;
    string         cmdName, outFileName = "", code;
    vector<string> fileName;
    unsigned int   n = 0;

    opt.addOption("o", "output", OptParser::OptType::value  , true,
                  "output file name (default: result not saved)");
    opt.addOption("" , "help"  , OptParser::OptType::trigger, true,
                  "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (opt.getArgs().size() >= 1)
    {
        n = strTo<unsigned int>(opt.getArgs()[0]);
    }
    else
    {
        parsed = false;
    }
    if (!parsed or (opt.getArgs().size() != n + 2) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0];
        cerr << " <options> <n> <function> <sample 1> ... <sample n>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    if (opt.gotOption("o"))
    {
        outFileName = opt.optionValue("o");
    }
    code = opt.getArgs()[1];
    for (unsigned int i = 0; i < n; ++i)
    {
        fileName.push_back(opt.getArgs()[2 + i]);
    }
    
    // process data ////////////////////////////////////////////////////////////
    try
    {
        process<DSample>(outFileName, fileName, code);
    }
    catch (Exceptions::Definition &)
    {
        process<DMatSample>(outFileName, fileName, code);
    }
    
    return EXIT_SUCCESS;
}
