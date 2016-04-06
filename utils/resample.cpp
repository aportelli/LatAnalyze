/*
 * resample.cpp, part of LatAnalyze 3
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
#include <map>
#include <libgen.h>
#include <unistd.h>
#include <LatAnalyze/Dataset.hpp>
#include <LatAnalyze/Io.hpp>

#ifndef DEF_NSAMPLE
#define DEF_NSAMPLE 100
#endif

using namespace std;
using namespace Latan;

static void usage(const string &cmdName)
{
    cerr << "usage: " << cmdName;
    cerr << " [-n <nsample> -b <bin size> -r <seed> -o <output dir> -f {h5|sample}]";
    cerr << " <data list> <name list>";
    cerr << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    int           c;
    random_device rd;
    SeedType      seed = rd();
    string        manFileName, nameFileName, cmdName, outDirName = ".";
    string        ext = "h5";
    Index         binSize = 1, nSample = DEF_NSAMPLE;
    
    opterr = 0;
    cmdName = basename(argv[0]);
    while ((c = getopt(argc, argv, "b:n:r:o:f:")) != -1)
    {
        switch (c)
        {
            case 'b':
                binSize = strTo<Index>(optarg);
                break;
            case 'n':
                nSample = strTo<Index>(optarg);
                break;
            case 'o':
                outDirName = optarg;
                break;
            case 'r':
                seed = strTo<SeedType>(optarg);
                break;
            case 'f':
                ext = optarg;
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
    if (argc - optind == 2)
    {
        manFileName  = argv[optind];
        nameFileName = argv[optind+1];
    }
    else
    {
        usage(cmdName);
    }
    
    // parameter parsing ///////////////////////////////////////////////////////
    vector<string> dataFileName, name;
    
    dataFileName = readManifest(manFileName);
    name         = readManifest(nameFileName);
    cout << "================================================" << endl;
    cout << "Data resampler" << endl;
    cout << "------------------------------------------------" << endl;
    cout << "        #file= " << dataFileName.size() << endl;
    cout << "        #name= " << name.size() << endl;
    cout << "     bin size= " << binSize << endl;
    cout << "      #sample= " << nSample << endl;
    cout << "   output dir: " << outDirName << endl;
    cout << "output format: " << ext << endl;
    cout << "------------------------------------------------" << endl;
    
    // data loading ////////////////////////////////////////////////////////////
    map<string, Dataset<DMat>> data;
    
    cout << "-- loading data..." << endl;
    for (const string &n: name)
    {
        data[n].resize(static_cast<Index>(dataFileName.size()));
    }
    for (unsigned int i = 0; i < dataFileName.size(); ++i)
    {
        std::unique_ptr<File> dataFile = Io::open(dataFileName[i]);

        cout << '\r' << ProgressBar(i + 1, dataFileName.size());
        for (const string &n: name)
        {
            data[n][i] = dataFile->read<DMat>(n);
        }
    }
    cout << endl;
    
    // data resampling /////////////////////////////////////////////////////////
    DMatSample   s(nSample);
    
    cout << "-- resampling data..." << endl;
    for (unsigned int i = 0; i < name.size(); ++i)
    {
        const string outFileName = name[i] + "_" + manFileName + "." + ext;
        
        cout << '\r' << ProgressBar(i + 1, name.size());
        data[name[i]].bin(binSize);
        s = data[name[i]].bootstrapMean(nSample, seed);
        Io::save<DMatSample>(s, outDirName + "/" + outFileName,
                             File::Mode::write, outFileName);
    }
    cout << endl;
    
    return EXIT_SUCCESS;
}
