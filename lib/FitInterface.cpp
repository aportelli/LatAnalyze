/*
 * FitInterface.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/FitInterface.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                     FitInterface implementation                            *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
FitInterface::FitInterface(void)
{}

// add dimensions //////////////////////////////////////////////////////////////
void FitInterface::addXDim(const string name, const Index nData)
{
    xDimName_.push_back(name);
    xSize_.push_back(nData);
    xIndex_[name] = xDimName_.size();
    updateYSize();
}

void FitInterface::addYDim(const string name)
{
    yDimName_.push_back(name);
    yIndex_[name] = yDimName_.size();
}

unsigned int FitInterface::getNXDim(void)
{
    return xDimName_.size();
}

unsigned int FitInterface::getNYDim(void)
{
    return yDimName_.size();
}

void FitInterface::updateYSize(void)
{
    ySize_ = 1;
    for (Index size: xSize_)
    {
        ySize_ *= size;
    }
    isFitPoint_.resize(ySize_);
    isFitPoint_.fill(1);
}

// enable fit points ///////////////////////////////////////////////////////////
void FitInterface::fitPoint(const bool isFitPoint, const Index j)
{
    DEBUG_VAR(j);
    isFitPoint_(j) = isFitPoint ? 1 : 0;
}
