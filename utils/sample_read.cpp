/*
 * sample_read.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2014 Antonin Portelli
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
#include <LatAnalyze/AsciiFile.hpp>
#include <LatAnalyze/Plot.hpp>

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
    
    cout << "-- loading sample from '" << fileName << "'..." << endl;
    const DMatSample &s = Io::load<DMatSample, AsciiFile>(fileName);
    cout << scientific;
    cout << "central value:\n"      << s[central]               << endl;
    cout << "standard deviation:\n" << s.variance().cwiseSqrt() << endl;
    
    return EXIT_SUCCESS;
}
