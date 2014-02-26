#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <latan/AsciiFile.hpp>
#include <latan/Dataset.hpp>

#ifndef DEF_NSAMPLE
#define DEF_NSAMPLE 100
#endif

using namespace std;
using namespace Latan;

static void usage(const string &cmdName)
{
    cerr << "usage: " << cmdName
         << " [-n <nsample> -r <state> -o <output>] <manifest> <name> "
         << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int c;
    Index nSample = DEF_NSAMPLE;
    string manFileName, name, outFileName, stateFileName;
    char *cmdName;
    
    opterr = 0;
    cmdName = basename(argv[0]);
    while ((c = getopt(argc, argv, "n:r:o:")) != -1)
    {
        switch (c)
        {
            case 'n':
                nSample = strTo<Index>(optarg);
                break;
            case 'r':
                stateFileName = optarg;
                break;
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
    if (argc - optind == 2)
    {
        manFileName = argv[optind];
        name        = argv[optind+1];
    }
    else
    {
        usage(cmdName);
    }
    
    Dataset<DMat> dataset;
    DMatSample s;
    RandGen g;
    
    if (!stateFileName.empty())
    {
        AsciiFile f(stateFileName, File::Mode::read);
        g.setState(f.read<RandGenState>());
    }
    cout << "-- loading data from manifest '" << manFileName << "'..." << endl;
    dataset.load<AsciiFile>(manFileName, name);
    s = dataset.bootstrapMean(nSample, g);
    cout << scientific;
    cout << "central value:\n"      << s[central]               << endl;
    cout << "standard deviation:\n" << s.variance().cwiseSqrt() << endl;
    if (!outFileName.empty())
    {
        Io::save<DMatSample, AsciiFile>(s, outFileName, File::Mode::write,
                                        manFileName + "_" + name);
    }
    
    return EXIT_SUCCESS;
}
