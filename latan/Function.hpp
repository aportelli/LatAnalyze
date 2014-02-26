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
#include <latan/Mat.hpp>
#include <functional>
#include <memory>
#include <stack>
#include <vector>

BEGIN_NAMESPACE

/******************************************************************************
 *                            Double function class                           *
 ******************************************************************************/
class DoubleFunction
{
private:
    typedef std::function<double(const double *)> vecFunc;
public:
    // constructor
    DoubleFunction(const Index nArg = 0, const vecFunc &f = nullFunction_);
    // destructor
    virtual ~DoubleFunction(void) = default;
    // access
    Index getNArg(void) const;
    void  setFunction(const vecFunc &f);
    void  setNArg(const Index nArg);
    // function call
    double operator()(const DVec &arg) const;
    double operator()(const std::vector<double> &arg) const;
    double operator()(std::stack<double> &arg) const;
    double operator()(const double x0, ...) const;
protected:
    // function call
    virtual double operator()(const double *arg) const;
private:
    std::shared_ptr<DVec> buffer_;
    vecFunc               f_;
    static const vecFunc  nullFunction_;
};

END_NAMESPACE

#endif // Latan_Function_hpp_
