/*
 * resample.cpp, part of LatAnalyze 3
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
#include <LatAnalyze/Statistics/Dataset.hpp>
#include <LatAnalyze/Io/Io.hpp>

#ifndef DEF_NSAMPLE
#define DEF_NSAMPLE "100"
#endif
#define DEF_FMT "h5"

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    // argument parsing ////////////////////////////////////////////////////////
    OptParser     opt;
    bool          parsed, dumpBoot;
    random_device rd;
    SeedType      seed = rd();
    string        manFileName, nameFileName, outDirName;
    string        ext;
    Index         binSize, nSample;
    
    opt.addOption("n", "nsample"   , OptParser::OptType::value,   true,
                  "number of samples", DEF_NSAMPLE);
    opt.addOption("b", "bin"       , OptParser::OptType::value,   true,
                  "bin size", "1");
    opt.addOption("r", "seed"      , OptParser::OptType::value,   true,
                  "random generator seed (default: random)");
    opt.addOption("o", "output-dir", OptParser::OptType::value,   true,
                  "output directory", ".");
    opt.addOption("f", "format"    , OptParser::OptType::value,   true,
                  "output file format", DEF_FMT);
    opt.addOption("d", "dump-boot" , OptParser::OptType::trigger, true,
                  "dump bootstrap sequence");
    opt.addOption("" , "help"      , OptParser::OptType::trigger, true,
                  "show this help message and exit");
    parsed = opt.parse(argc, argv);
    if (!parsed or (opt.getArgs().size() != 2) or opt.gotOption("help"))
    {
        cerr << "usage: " << argv[0];
        cerr << " <datafile list> <name list> <options>" << endl;
        cerr << endl << "Possible options:" << endl << opt << endl;
        
        return EXIT_FAILURE;
    }
    nSample = opt.optionValue<Index>("n");
    binSize = opt.optionValue<Index>("b");
    if (opt.gotOption("r"))
    {
        seed = opt.optionValue<SeedType>("r");
    }
    ext          = opt.optionValue("f");
    outDirName   = opt.optionValue("o");
    dumpBoot     = opt.gotOption("d");
    manFileName  = opt.getArgs()[0];
    nameFileName = opt.getArgs()[1];
    
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
        if ((i == 0) and dumpBoot)
        {
            ofstream file(outDirName + "/" + manFileName + ".bootseq");

            file << "# bootstrap sequences" << endl;
            file << "# manifest file: " << manFileName << endl;
            file << "#      bin size: " << binSize << endl;
            data[name[i]].dumpBootstrapSeq(file, nSample, seed);
        }
        s = data[name[i]].bootstrapMean(nSample, seed);
        Io::save<DMatSample>(s, outDirName + "/" + outFileName,
                             File::Mode::write, outFileName);
    }
    cout << endl;
    
    return EXIT_SUCCESS;
}
