/*
 * TabFunction.hpp, part of LatAnalyze 3
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

#ifndef Latan_TabFunction_hpp_
#define Latan_TabFunction_hpp_

#include <algorithm>
#include <map>
#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/XYStatData.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                      tabulated function: 1D only                           *
 ******************************************************************************/

enum class InterpType
{
  NEAREST,
  LINEAR,
  QUADRATIC
};

class TabFunction: public DoubleFunctionFactory
{
public:
    // constructors
    TabFunction(void) = default;
    TabFunction(const DVec &x, const DVec &y,
                const InterpType interpType = InterpType::LINEAR);
    TabFunction(const XYStatData &data, const Index i = 0, const Index j = 0,
                const InterpType interpType = InterpType::LINEAR);
    // destructor
    virtual ~TabFunction(void) = default;
    // access
    void setData(const DVec &x, const DVec &y);
    void setData(const XYStatData &data, const Index i = 0, const Index j = 0);
    // function call
    double operator()(const double *arg) const;
    // factory
    virtual DoubleFunction makeFunction(const bool makeHardCopy = true) const;
private:
    std::map<double, double>::const_iterator nearest(const double x) const;

    std::map<double, double> value_;
    InterpType interpType_;
};

DoubleFunction interpolate(const DVec &x, const DVec &y,
                           const InterpType interpType = InterpType::LINEAR);
DoubleFunction interpolate(const XYStatData &data, const Index i = 0,
                           const Index j = 0,
                           const InterpType interpType = InterpType::LINEAR);

END_LATAN_NAMESPACE

#endif // Latan_TabFunction_hpp_
