/*
 * TabFunction.hpp, part of LatAnalyze 3
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

#ifndef Latan_TabFunction_hpp_
#define Latan_TabFunction_hpp_

#include <map>
#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/XYStatData.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                      tabulated function: 1D only                           *
 ******************************************************************************/

class TabFunction: public DoubleFunction
{
public:
    // constructors
    TabFunction(void);
    TabFunction(const DVec &x, const DVec &y);
    TabFunction(const XYStatData &data, const Index i = 0, const Index j = 0);
    // destructor
    virtual ~TabFunction(void) = default;
    // access
    void setData(const DVec &x, const DVec &y);
    void setData(const XYStatData &data, const Index i = 0, const Index j = 0);
    // function call
    using DoubleFunction::operator();
    virtual double operator()(const double *arg) const;
private:
    std::map<double, double> value_;
};

END_LATAN_NAMESPACE

#endif // Latan_TabFunction_hpp_
