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
 *                            Function implementation                         *
 ******************************************************************************/
// constructor/destructor //////////////////////////////////////////////////////
Function::Function(const unsigned nArg)
: nArg_(nArg)
{}

Function::~Function(void)
{}

// access //////////////////////////////////////////////////////////////////////
unsigned int Function::getNArg(void) const
{
    return nArg_;
}

/******************************************************************************
 *                        DoubleFunction implementation                       *
 ******************************************************************************/
DoubleFunction::DoubleFunction(const unsigned nArg)
: Function(nArg), buffer_(nArg)
{}

DoubleFunction::~DoubleFunction(void)
{}

double DoubleFunction::operator()(std::stack<double> &arg)
{
    for (unsigned int i = 0; i < getNArg(); ++i)
    {
        buffer_[getNArg() - i - 1] = arg.top();
        arg.pop();
    }
    
    return (*this)(buffer_);
}

double DoubleFunction::operator()(const double x0, ...)
{
    buffer_[0] = x0;
    if (getNArg() > 1)
    {
        va_list va;
        
        va_start(va, x0);
        for (unsigned int i = 1; i < getNArg(); ++i)
        {
            buffer_[i] = va_arg(va, double);
        }
        va_end(va);
    }
    
    return (*this)(buffer_);
}
