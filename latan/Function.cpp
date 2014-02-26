/*
 * Function.cpp, part of LatAnalyze 3
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

#include <latan/Function.hpp>
#include <latan/includes.hpp>

#define DEF_STD_FUNC_1ARG(name, funcName) \
name##Function::name##Function(void): DoubleFunction(1) {}\
name##Function::~name##Function(void) {}\
double name##Function::operator()(std::vector<double> &arg)\
{\
    return funcName(arg[0]);\
}\
name##Function STDMATH_NAMESPACE::funcName;

#define DEF_STD_FUNC_2ARG(name, funcName) \
name##Function::name##Function(void): DoubleFunction(2) {}\
name##Function::~name##Function(void) {}\
double name##Function::operator()(std::vector<double> &arg)\
{\
return funcName(arg[0], arg[1]);\
}\
name##Function STDMATH_NAMESPACE::funcName;

using namespace std;
using namespace Latan;

/******************************************************************************
 *                        DoubleFunction implementation                       *
 ******************************************************************************/
const DoubleFunction::vecFunc DoubleFunction::nullFunction_ = nullptr;

// constructor /////////////////////////////////////////////////////////////////
DoubleFunction::DoubleFunction(const Index nArg, const vecFunc &f)
: buffer_(new DVec(nArg))
, f_(f)
{}

// access //////////////////////////////////////////////////////////////////////
Index DoubleFunction::getNArg(void) const
{
    return buffer_->size();
}

void DoubleFunction::setFunction(const vecFunc &f)
{
    f_ = f;
}

void DoubleFunction::setNArg(const Index nArg)
{
    buffer_->resize(nArg);
}

// function call ///////////////////////////////////////////////////////////////
double DoubleFunction::operator()(const double *arg) const
{
    return f_(arg);
}

double DoubleFunction::operator()(const DVec &arg) const
{
    if (arg.size() != getNArg())
    {
        LATAN_ERROR(Size, "function argument vector has a wrong size (expected "
                    + strFrom(getNArg()) + ", got " + strFrom(arg.size())
                    + ")");
    }
    
    return (*this)(arg.data());
}

double DoubleFunction::operator()(const std::vector<double> &arg) const
{
    if (arg.size() != static_cast<unsigned int>(getNArg()))
    {
        LATAN_ERROR(Size, "function argument vector has a wrong size (expected "
                    + strFrom(getNArg()) + ", got " + strFrom(arg.size())
                    + ")");
    }
    
    return (*this)(arg.data());
}

double DoubleFunction::operator()(std::stack<double> &arg) const
{
    for (Index i = 0; i < getNArg(); ++i)
    {
        if (arg.empty())
        {
            LATAN_ERROR(Size, "function argument stack has a wrong size (expected "
                        + strFrom(getNArg()) + ", got " + strFrom(i)
                        + ")");
        }
        (*buffer_)(getNArg() - i - 1) = arg.top();
        arg.pop();
    }
    
    return (*this)(*buffer_);
}

double DoubleFunction::operator()(const double x0, ...) const
{
    (*buffer_)[0] = x0;
    if (getNArg() > 1)
    {
        va_list va;
        
        va_start(va, x0);
        for (Index i = 1; i < getNArg(); ++i)
        {
            (*buffer_)(i) = va_arg(va, double);
        }
        va_end(va);
    }
    
    return (*this)(*buffer_);
}
