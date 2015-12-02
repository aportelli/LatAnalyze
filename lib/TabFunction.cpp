/*
 * TabFunction.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli
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
TabFunction::TabFunction(const DVec &x, const DVec &y,
                         const InterpType interpType)
: interpType_(interpType)
{
    setData(x, y);
}

TabFunction::TabFunction(const XYStatData &data, const Index i, const Index j,
                         const InterpType interpType)
: interpType_(interpType)
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
    double result = 0.0, x = arg[0];


    if ((x < value_.begin()->first) || (x >= value_.rbegin()->first)) {
        LATAN_ERROR(Range, "tabulated function variable out of range "
                               "(x= " + strFrom(x) + " not in ["
                           + strFrom(value_.begin()->first) + ", "
                           + strFrom(value_.rbegin()->first) + "])");
    }

    auto i = value_.equal_range(x);
    auto low = (x == i.first->first) ? i.first : prev(i.first);
    auto high = i.second;

    switch (interpType_) {
        case InterpType::LINEAR: {
            double x_a, x_b, y_a, y_b;

            x_a = low->first;
            x_b = high->first;
            y_a = low->second;
            y_b = high->second;
            result = y_a + (x - x_a) * (y_b - y_a) / (x_b - x_a);
            break;
        }
        case InterpType::NEAREST: {
            result = nearest(x)->second;
            break;
        }
        case InterpType::QUADRATIC: {
            double xs[3], ys[3], ds[3], d01, d02, d12;
            auto it = nearest(x);
            if (it == value_.begin()) {
                it = next(it);
            }
            else if (it == prev(value_.end())) {
                it = prev(it);
            }
            xs[0] = prev(it)->first;
            ys[0] = prev(it)->second;
            xs[1] = it->first;
            ys[1] = it->second;
            xs[2] = next(it)->first;
            ys[2] = next(it)->second;
            ds[0] = x - xs[0];
            ds[1] = x - xs[1];
            ds[2] = x - xs[2];
            d01   = xs[0] - xs[1];
            d02   = xs[0] - xs[2];
            d12   = xs[1] - xs[2];

            // Lagrange polynomial coefficient computation
            result = ds[1]/d01*ds[2]/d02*ys[0]
                     -ds[0]/d01*ds[2]/d12*ys[1]
                     +ds[0]/d02*ds[1]/d12*ys[2];
            break;
        }
        default:
            int intType = static_cast<int>(interpType_);
            LATAN_ERROR(Implementation, "unsupported interpolation type in "
                                        "tabulated function: "
                                        + strFrom(intType));
    }
    
    return result;
}

// DoubleFunction factory //////////////////////////////////////////////////////
DoubleFunction TabFunction::makeFunction(const bool makeHardCopy) const
{
    DoubleFunction res;

    if (makeHardCopy)
    {
        TabFunction copy(*this);

        res.setFunction([copy](const double *x){return copy(x);}, 1);
    }
    else
    {
        res.setFunction([this](const double *x){return (*this)(x);}, 1);
    }

    return res;
}

DoubleFunction Latan::interpolate(const DVec &x, const DVec &y,
                                  const InterpType interpType)
{
    return TabFunction(x, y, interpType).makeFunction();
}

DoubleFunction Latan::interpolate(const XYStatData &data, const Index i,
                                  const Index j, const InterpType interpType)
{
    return TabFunction(data, i, j, interpType).makeFunction();
}

map<double, double>::const_iterator TabFunction::nearest(const double x) const
{
    map<double, double>::const_iterator ret;
    auto i = value_.equal_range(x);
    auto low = (x == i.first->first) ? i.first : prev(i.first);
    auto high = i.second;
    if (fabs(high->first - x) < fabs(low->first - x)) {
        ret = high;
    }
    else {
        ret = low;
    }

    return ret;
}
