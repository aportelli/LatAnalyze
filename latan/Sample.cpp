/*
 * Sample.cpp, part of LatAnalyze 3
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

#include <latan/Sample.hpp>
#include <latan/includes.hpp>

using namespace Latan;

DSample::DSample(void)
: DSampleBase(static_cast<Index>(0))
{}

DSample::DSample(const unsigned int nSample, const unsigned int nRow,
                 const unsigned int nCol)
: DSampleBase(static_cast<Index>(nSample + 1))
{
    for (int s = 0; s < size(); ++s)
    {
        (*this)(s).resize(nRow, nCol);
    }
}

DMat& DSample::operator()(const int s)
{
    if (s >= 0)
    {
        return (*this)(s + 1);
    }
    else
    {
        return (*this)(0);
    }
}
