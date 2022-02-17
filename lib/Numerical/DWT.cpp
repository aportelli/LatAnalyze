/*
 * DWT.cpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <LatAnalyze/Numerical/DWT.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                            DWT implementation                              *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
DWT::DWT(const DWTFilter &filter)
: filter_(filter)
{}

// convolution primitive ///////////////////////////////////////////////////////
DVec DWT::filterConvolution(const DVec &data, const DWTFilter &filter, 
                            const Index offset)
{
    DVec res(data.size());

    return res;
}

// DWT /////////////////////////////////////////////////////////////////////////
std::vector<DWT::DWTLevel> 
DWT::forward(const DVec &data, const unsigned int level) const
{
    std::vector<DWT::DWTLevel> dwt(level);

    return dwt;
}

DVec DWT::backward(const std::vector<DWTLevel>& dwt) const
{
    DVec res;

    return res;
}
