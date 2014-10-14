/*
 * TabFunction.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/TabFunction.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                       TabFunction implementation                           *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
TabFunction::TabFunction(void)
: DoubleFunction(1)
{}

TabFunction::TabFunction(const DVec &x, const DVec &y)
: TabFunction()
{
    setData(x, y);
}

TabFunction::TabFunction(const XYStatData &data, const Index i, const Index j)
: TabFunction()
{
    setData(data, i, j);
}

// access //////////////////////////////////////////////////////////////////////
void TabFunction::setData(const DVec &x, const DVec &y)
{
    if (x.size() != y.size())
    {
        LATAN_ERROR(Size, "tabulated function x/y data size mismatch");
    }
    FOR_VEC(x, i)
    {
        value_[x(i)] = y(i);
    }
}

void TabFunction::setData(const XYStatData &data, const Index i, const Index j)
{
    setData(data.x(i), data.y(j));
}

// function call ///////////////////////////////////////////////////////////////
double TabFunction::operator()(const double *arg) const
{
    double x = arg[0], x_a, x_b, y_a, y_b;
    
    if ((x < value_.begin()->first)||(x >= value_.rbegin()->first))
    {
        LATAN_ERROR(Range, "tabulated function variable out of range (x= "
                    + strFrom(x) + " not in ["
                    + strFrom(value_.begin()->first) + ", "
                    + strFrom(value_.rbegin()->first) + "])");
    }
    
    auto i    = value_.equal_range(x);
    auto low  = (x == i.first->first) ? i.first : prev(i.first);
    auto high = i.second;
    
    x_a = low->first;
    x_b = high->first;
    y_a = low->second;
    y_b = high->second;
    
    return y_a + (x - x_a)*(y_b - y_a)/(x_b - x_a);
}
