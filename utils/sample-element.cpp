#include <iostream>
#include <string>

#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/MatSample.hpp>


int main(int argc, char* argv[])
{
    using namespace std;
    using namespace Latan;

    if (argc != 4 and argc != 5) {
        cout << "Usage: " << argv[0] << " <input filename> <row> <column> ";
        cout << "[output filename]" << endl;
        return -1;
    }

    string inFileName = argv[1];
    auto row = strTo<Index>(argv[2]);
    auto col = strTo<Index>(argv[3]);
    string outFileName = (argc == 5) ? argv[4] : "";

    auto inputData = Io::load<DMatSample>(inFileName);
    cout << scientific;
    cout << "central value:\n" << inputData[central](row, col) << endl;
    cout << "standard deviation:\n";
    cout << inputData.variance().cwiseSqrt()(row, col) << endl;

    if (not outFileName.empty())
    {
        DSample outputData(inputData.size());
        FOR_STAT_ARRAY(inputData, s) {
            outputData[s] = inputData[s](row, col);
        }

        Io::save(outputData, outFileName);
    }
}
