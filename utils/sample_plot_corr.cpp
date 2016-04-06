/*
 * sample_plot_corr.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
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

#include <LatAnalyze/Io.hpp>
#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/Plot.hpp>

using namespace std;
using namespace Latan;
using namespace Math;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "usage: " << argv[0] << " <file>" << endl;
        
        return EXIT_FAILURE;
    }
    
    string     fileName = argv[1], name;
    DMatSample sample;
    DMat       var;
    Plot       p;
    
    cout << "-- computing variance matrix from '" << fileName << "'..." << endl;
    name   = Io::getFirstName(fileName);
    sample = Io::load<DMatSample>(fileName);
    var    = sample.varianceMatrix();
    p << PlotMatrix(varToCorr(var));
    p.display();
    
    return EXIT_SUCCESS;
}
