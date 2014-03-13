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

using namespace std;
using namespace Latan;

/******************************************************************************
 *                        DoubleFunction implementation                       *
 ******************************************************************************/
const DoubleFunction::vecFunc DoubleFunction::nullFunction_ = nullptr;

// constructor /////////////////////////////////////////////////////////////////
DoubleFunction::DoubleFunction(const Index nArg, const vecFunc &f)
: buffer_(new DVec)
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

/******************************************************************************
 *                    DoubleFunctionSample implementation                     *
 ******************************************************************************/
DSample DoubleFunctionSample::operator()(const DMatSample &arg) const
{
    DSample result(arg.size());
    
    FOR_STAT_ARRAY(arg, s)
    {
        result[s] = (*this)[s](arg[s]);
    }
    
    return result;
}
