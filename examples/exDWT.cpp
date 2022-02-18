#include <LatAnalyze/Numerical/DWT.hpp>

using namespace std;
using namespace Latan;

int main(void)
{
    DVec                  data, dataRec;
    vector<DWT::DWTLevel> dataDWT;
    DWT                   dwt(DWTFilters::db3);

    cout << "-- random data" << endl;
    data.setRandom(16);
    cout << data.transpose() << endl;
    cout << "-- compute Daubechies 3 DWT" << endl;
    dataDWT = dwt.forward(data, 4);
    for (unsigned int l = 0; l < dataDWT.size(); ++l)
    {
        cout << "* level " << l << endl;
        cout << "L= " << dataDWT[l].first.transpose() << endl;
        cout << "H= " << dataDWT[l].second.transpose() << endl;
    }
    cout << "-- check inverse DWT" << endl;
    dataRec = dwt.backward(dataDWT);
    cout << "rel diff = " << 2.*(data - dataRec).norm()/(data + dataRec).norm() << endl;

    return EXIT_SUCCESS;
}
