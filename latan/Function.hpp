/*
 * Function.hpp, part of LatAnalyze 3
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

#ifndef Latan_Function_hpp_
#define	Latan_Function_hpp_

#include <latan/Global.hpp>
#include <stack>
#include <vector>
#include <cstdarg>

BEGIN_NAMESPACE

/******************************************************************************
 *                            Base function class                             *
 ******************************************************************************/
class Function
{
public:
    // constructor/destructor
    explicit Function(const unsigned nArg);
    virtual ~Function(void);
    // access
    unsigned int getNArg(void) const;
private:
    const unsigned int nArg_;
};

/******************************************************************************
 *                            Double function class                           *
 ******************************************************************************/
class DoubleFunction: public Function
{
public:
    // constructor/destructor
    explicit DoubleFunction(const unsigned nArg);
    virtual ~DoubleFunction(void);
    // function call
    virtual double operator()(std::vector<double> &arg) = 0;
    double operator()(std::stack<double> &arg);
    double operator()(const double x0, ...);
private:
    std::vector<double> buffer_;
};

END_NAMESPACE

#endif // Latan_Function_hpp_
