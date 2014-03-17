/*
 * Model.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Model.hpp>
#include <LatAnalyze/includes.hpp>
#include <functional>

using namespace std;
using namespace std::placeholders;
using namespace Latan;

/******************************************************************************
 *                           Model implementation                             *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
DoubleModel::DoubleModel(const Index nArg, const Index nPar, const vecFunc &f)
: size_(new ModelSize)
{
    setFunction(nArg, nPar, f);
}

// access //////////////////////////////////////////////////////////////////////
Index DoubleModel::getNArg(void) const
{
    return size_->nArg;
}

Index DoubleModel::getNPar(void) const
{
    return size_->nPar;
}

void DoubleModel::setFunction(const Index nArg, const Index nPar,
                              const vecFunc &f)
{
    size_->nArg = nArg;
    size_->nPar = nPar;
    f_          = f;
}

// error checking //////////////////////////////////////////////////////////////
void DoubleModel::checkSize(const Index nArg, const Index nPar) const
{
    if (nArg != getNArg())
    {
        LATAN_ERROR(Size, "model argument vector has a wrong size (expected "
                    + strFrom(getNArg()) + ", got " + strFrom(nArg)
                    + ")");
    }
    if (nPar != getNPar())
    {
        LATAN_ERROR(Size, "model parameter vector has a wrong size (expected "
                    + strFrom(getNPar()) + ", got " + strFrom(nPar)
                    + ")");
    }
}

// function call ///////////////////////////////////////////////////////////////
double DoubleModel::operator()(const DVec &arg, const DVec &par) const
{
    checkSize(arg.size(), par.size());
    
    return (*this)(arg.data(), par.data());
}

double DoubleModel::operator()(const vector<double> &arg,
                               const vector<double> &par) const
{
    checkSize(static_cast<Index>(arg.size()), static_cast<Index>(par.size()));
    
    return (*this)(arg.data(), par.data());
}

double DoubleModel::operator()(const double *data, const double *par) const
{
    return f_(data, par);
}

// model bind //////////////////////////////////////////////////////////////////
DoubleFunction DoubleModel::getBind(const DVec &par) const
{
    auto modelWithVec = [this](const double *_arg, const DVec &_par)
                              {return (*this)(_arg, _par.data());};
    auto modelBind    = bind(modelWithVec, _1, par);

    return DoubleFunction(getNArg(), modelBind);
}
