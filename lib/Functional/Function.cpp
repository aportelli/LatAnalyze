/*
 * Function.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
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

#include <LatAnalyze/Functional/Function.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                        DoubleFunction implementation                       *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
DoubleFunction::DoubleFunction(const vecFunc &f, const Index nArg)
: buffer_(new DVec)
, varName_("x")
{
    setFunction(f, nArg);
}

// access //////////////////////////////////////////////////////////////////////
Index DoubleFunction::getNArg(void) const
{
    return buffer_->size();
}

void DoubleFunction::setFunction(const vecFunc &f, const Index nArg)
{
    buffer_->resize(nArg);
    f_ = f;
}

VarName & DoubleFunction::varName(void)
{
    return varName_;
}

const VarName & DoubleFunction::varName(void) const
{
    return varName_;
}

// error checking //////////////////////////////////////////////////////////////
void DoubleFunction::checkSize(const Index nPar) const
{
    if (nPar != getNArg())
    {
        LATAN_ERROR(Size, "function argument vector has a wrong size (expected "
                    + strFrom(getNArg()) + ", got " + strFrom(nPar)
                    + ")");
    }
}

// function call ///////////////////////////////////////////////////////////////
double DoubleFunction::operator()(const double *arg) const
{
    return f_(arg);
}

double DoubleFunction::operator()(const DVec &arg) const
{
    checkSize(arg.size());
    
    return (*this)(arg.data());
}

double DoubleFunction::operator()(const std::vector<double> &arg) const
{
    checkSize(static_cast<Index>(arg.size()));
    
    return (*this)(arg.data());
}

double DoubleFunction::operator()(std::stack<double> &arg) const
{
    for (Index i = 0; i < getNArg(); ++i)
    {
        if (arg.empty())
        {
            LATAN_ERROR(Size, "function argument stack is empty (expected "
                        + strFrom(getNArg()) + "arguments, got " + strFrom(i)
                        + ")");
        }
        (*buffer_)(getNArg() - i - 1) = arg.top();
        arg.pop();
    }
    
    return (*this)(*buffer_);
}

double DoubleFunction::operator()(void) const
{
    checkSize(0);
    
    return (*this)(nullptr);
}

// bind ////////////////////////////////////////////////////////////////////////
DoubleFunction DoubleFunction::bind(const Index argIndex,
                                    const double val) const
{
    Index            nArg = getNArg();
    shared_ptr<DVec> buf(new DVec(nArg));
    DoubleFunction   copy(*this), bindFunc;

    auto func = [copy, buf, argIndex, val](const double *arg)
    {
        FOR_VEC(*buf, i)
        {
            if (i < argIndex)
            {
                (*buf)(i) = arg[i];
            }
            else if (i == argIndex)
            {
                (*buf)(i) = val;
            }
            else
            {
                (*buf)(i) = arg[i - 1];
            }
        }

        return copy(*buf);
    };

    bindFunc.setFunction(func, nArg - 1);

    return bindFunc;
}

DoubleFunction DoubleFunction::bind(const Index argIndex,
                                    const DVec &x) const
{
    Index            nArg = getNArg();
    shared_ptr<DVec> buf(new DVec(nArg));
    DoubleFunction   copy(*this), bindFunc;

    auto func = [copy, buf, argIndex, x](const double *arg)
    {
        *buf = x;
        (*buf)(argIndex) = arg[0];

        return copy(*buf);
    };

    bindFunc.setFunction(func, 1);

    return bindFunc;
}

// arithmetic operators ////////////////////////////////////////////////////////
DoubleFunction DoubleFunction::operator-(void) const
{
    DoubleFunction copy(*this), resFunc;
    
    return DoubleFunction([copy](const double *arg){return -copy(arg);},
                          getNArg());
}

#define MAKE_SELF_FUNC_OP(op)\
DoubleFunction & DoubleFunction::operator op##=(const DoubleFunction &f)\
{\
    DoubleFunction copy(*this);\
    checkSize(f.getNArg());\
    auto res = [f, copy](const double *arg){return copy(arg) op f(arg);};\
    setFunction(res, getNArg());\
    return *this;\
}\
DoubleFunction & DoubleFunction::operator op##=(const DoubleFunction  and f)\
{\
    *this op##= f;\
    return *this;\
}

#define MAKE_SELF_SCALAR_OP(op)\
DoubleFunction & DoubleFunction::operator op##=(const double x)\
{\
    DoubleFunction copy(*this);\
    auto res = [x, copy](const double *arg){return copy(arg) op x;};\
    setFunction(res, getNArg());\
    return *this;\
}\

MAKE_SELF_FUNC_OP(+)
MAKE_SELF_FUNC_OP(-)
MAKE_SELF_FUNC_OP(*)
MAKE_SELF_FUNC_OP(/)
MAKE_SELF_SCALAR_OP(+)
MAKE_SELF_SCALAR_OP(-)
MAKE_SELF_SCALAR_OP(*)
MAKE_SELF_SCALAR_OP(/)

/******************************************************************************
 *                    DoubleFunctionSample implementation                     *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
DoubleFunctionSample::DoubleFunctionSample(void)
: Sample<DoubleFunction>()
{}

DoubleFunctionSample::DoubleFunctionSample(const Index nSample)
: Sample<DoubleFunction>(nSample)
{}

// function call ///////////////////////////////////////////////////////////////
DSample DoubleFunctionSample::operator()(const DMatSample &arg) const
{
    DSample result(size());
    
    FOR_STAT_ARRAY((*this), s)
    {
        result[s] = (*this)[s](arg[s]);
    }
    
    return result;
}

DSample DoubleFunctionSample::operator()(const double *arg) const
{
    DSample result(size());
    
    FOR_STAT_ARRAY((*this), s)
    {
        result[s] = (*this)[s](arg);
    }
    
    return result;
}

DSample DoubleFunctionSample::operator()(const DVec &arg) const
{
    return (*this)(arg.data());
}

DSample DoubleFunctionSample::operator()(const vector<double> &arg) const
{
    return (*this)(arg.data());
}

// bind ////////////////////////////////////////////////////////////////////////
DoubleFunctionSample DoubleFunctionSample::bind(const Index argIndex,
                                                const double val) const
{
    DoubleFunctionSample bindFunc(size());

    FOR_STAT_ARRAY(bindFunc, s)
    {
        bindFunc[s] = (*this)[s].bind(argIndex, val);
    }

    return bindFunc;
}

DoubleFunctionSample DoubleFunctionSample::bind(const Index argIndex,
                                                const DVec &x) const
{
    DoubleFunctionSample bindFunc(size());

    FOR_STAT_ARRAY(bindFunc, s)
    {
        bindFunc[s] = (*this)[s].bind(argIndex, x);
    }

    return bindFunc;
}
