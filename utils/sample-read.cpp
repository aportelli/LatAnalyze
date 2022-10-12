/*
 * sample_read.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Io/Io.hpp>

using namespace std;
using namespace Latan;

int main(int argc, char *argv[])
{
    if ((argc < 2) or (argc > 3))
    {
        cerr << "usage: " << argv[0] << " <file> [<copy>]" << endl;

        return EXIT_FAILURE;
    }

    string fileName = argv[1], copy = (argc >= 3) ? argv[2] : "";
    
    cout << "-- loading sample from '" << fileName << "'..." << endl;
    try
    {
        DMatSample s    = Io::load<DMatSample>(fileName);
        string     name = Io::getFirstName(fileName);
        Index nRows     = s[central].rows();
        Index nCols     = s[central].cols();
        cout << scientific;
        cout << "central value +/-  standard deviation\n" << endl;
        cout << "Re:" << endl;
        for(Index i = 0; i < nRows; i++)
        {
            cout << s[central](i,0) << " +/- " << s.variance().cwiseSqrt()(i,0) << endl;
        }
        if(nCols == 2)
        {
            cout << "\nIm:" << endl;
            for(Index i = 0; i < nRows; i++)
            {
                cout << s[central](i,1) << " +/- " << s.variance().cwiseSqrt()(i,1) << endl;
            }   
        }
        if (!copy.empty())
        {
            Io::save(s, copy, File::Mode::write, name);
        }
    }
    catch (Exceptions::Definition)
    {
        DSample s    = Io::load<DSample>(fileName);
        string  name = Io::getFirstName(fileName);
        cout << scientific;
        cout << "central value +/-  standard deviation\n" << endl;
        cout << s[central] << " +/- " << sqrt(s.variance()) << endl;
        if (!copy.empty())
        {
            Io::save(s, copy, File::Mode::write, name);
        }
    }
    
    return EXIT_SUCCESS;
}
