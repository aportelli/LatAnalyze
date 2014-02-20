#include <iostream>
#include <latan/AsciiFile.hpp>
#include <latan/RandGen.hpp>

using namespace std;
using namespace Latan;

const int    seqLength     = 25;
const int    saveStep      = 9;
const string stateFileName = "exRand.seed";

int main(void)
{
    RandGenState state;
    RandGen gen[2];
    AsciiFile stateFile(stateFileName, File::Mode::write|File::Mode::read);
    
    cout << "- GENERATOR STATE I/O TESTS" << endl;
    cout << "-- generating a " << seqLength << " steps random sequence..."
         << endl;
    for (int i = 0; i < seqLength; ++i)
    {
        if (i == saveStep)
        {
            state = gen[0].getState();
            stateFile.save(state, "exRand");
            cout << "generator state after step " << saveStep - 1
                 << " saved in '" << stateFileName << "'" << endl;
        }
        cout << "step " << i << "\t: " << gen[0].uniform() <<endl;
    }
    cout << "-- setting up another generator from '" << stateFileName << "'..."
         << endl;
    gen[1].setState(stateFile.read<RandGenState>("exRand"));
    cout << "-- generating a " << seqLength << " steps random sequence..."
         << endl;
    for (int i = 0; i < seqLength; ++i)
    {
        cout << "step " << i << "\t: " << gen[1].uniform() <<endl;
    }
    return EXIT_SUCCESS;
}
