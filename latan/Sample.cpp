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
using namespace std;

/******************************************************************************
 *                        DMatSample implementation                           *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
DMatSample::DMatSample(void)
: Sample<DMat>()
{}

DMatSample::DMatSample(const unsigned int nSample, const unsigned int nRow,
                       const unsigned int nCol)
: Sample<DMat>(nSample)
{
    resizeMat(nRow, nCol);
}

// resize all matrices /////////////////////////////////////////////////////////
void DMatSample::resizeMat(const unsigned int nRow, const unsigned int nCol)
{
    this->unaryExpr([nRow, nCol](DMat &m){m.resize(nRow, nCol);});
}

// IO type /////////////////////////////////////////////////////////////////////
IoObject::IoType DMatSample::getType(void) const
{
    return IoType::dMatSample;
}
