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
#include <functional>
#include <memory>
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
    // constructor
    explicit Function(const unsigned nArg);
    // destructor
    virtual ~Function(void) = default;
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
private:
    typedef std::function<double(const std::vector<double> &)> vecFunc;
public:
    // constructor
    explicit DoubleFunction(const unsigned nArg, vecFunc f = nullptr);
    // destructor
    virtual ~DoubleFunction(void) = default;
    // function call
    virtual double evaluate(const std::vector<double> &arg) const;
    double operator()(std::stack<double> &arg) const;
    double operator()(const double x0, ...) const;
private:
    std::shared_ptr<std::vector<double>> buffer_;
    vecFunc                              f_;
};

END_NAMESPACE

#endif // Latan_Function_hpp_
