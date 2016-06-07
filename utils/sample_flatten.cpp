#include <iostream>
#include <string>

#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/MatSample.hpp>


namespace La = Latan;

int main(int argc, char* argv[])
{  
  if (argc != 4) {
    std::cout << "Usage: " << argv[0] << " <input filename> <output filename> <num samples>" << std::endl;
    return -1;
  }

  std::string inFileName = argv[1];
  std::string outFileName = argv[2];
  auto nSamples = La::strTo<La::Index>(argv[3]);

  La::DMatSample inputData(nSamples, 1, 1);
  La::DSample outputData(nSamples);

  inputData = La::Io::load<La::DMatSample>(inFileName);

  FOR_STAT_ARRAY(inputData, s) {
    outputData[s] = inputData[s](0, 0);
  }

  La::Io::save(outputData, outFileName);
}
