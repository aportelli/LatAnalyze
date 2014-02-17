#include <iostream>
#include <latan/Io.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <file>" << endl;
        return EXIT_FAILURE;
    }
    
    string fileName = argv[1];
    AsciiFile f(fileName, File::Mode::read);
    
    cout << "-- loading sample from '" << fileName << "'..." << endl;
    const DMatSample &s = f.read<DMatSample>();
    cout << scientific;
    cout << "central value:\n"      << s[central]               << endl;
    cout << "standard deviation:\n" << s.variance().cwiseSqrt() << endl;
    
    return EXIT_SUCCESS;
}
