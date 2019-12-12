#include <LatAnalyze/Core/Math.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char* argv[])
{
    double chi2, ndof;

    if (argc != 3)
    {
        cerr << "usage: " << argv[0] << " <chi^2> <ndof>" << endl;
        
        return EXIT_FAILURE;
    }
    chi2 = strTo<double>(argv[1]);
    ndof = strTo<double>(argv[2]);

    cout << "Two-sided p-value: " << Math::chi2PValue(chi2, ndof) << endl;
    cout << "chi^2 CCDF       : " << Math::chi2Ccdf(chi2, ndof) << endl;

    return EXIT_SUCCESS;
}
