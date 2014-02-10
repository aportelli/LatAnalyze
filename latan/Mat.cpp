/*
 * Mat.cpp, part of LatAnalyze 3
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

#include <latan/Mat.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                                 DMat class                                 *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
DMat::DMat(void)
: DMatBase()
{}

DMat::DMat(const DMatBase &m)
: DMatBase(m)
{}

DMat::DMat(const DMat &m)
: DMatBase(m)
{}

DMat::DMat(const unsigned int nRow, const unsigned int nCol)
: DMatBase(nRow, nCol)
{}

unsigned int DMat::getType(void) const
{
    return IoType::dMat;
}
