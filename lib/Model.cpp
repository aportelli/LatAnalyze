/*
 * Model.cpp, part of LatAnalyze 3
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
DoubleModel::DoubleModel(const vecFunc &f, const Index nArg, const Index nPar)
: size_(new ModelSize)
{
    setFunction(f, nArg, nPar);
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

void DoubleModel::setFunction(const vecFunc &f, const Index nArg,
                              const Index nPar)
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
DoubleFunction DoubleModel::fixArg(const DVec &arg) const
{
    DoubleModel copy(*this);

    auto modelWithVec = [copy](const DVec &x, const double *p)
    {
        return copy(x.data(), p);
    };
    auto modelBind    = bind(modelWithVec, arg, _1);

    return DoubleFunction(modelBind, getNPar());
}

DoubleFunction DoubleModel::fixPar(const DVec &par) const
{
    DoubleModel copy(*this);

    auto modelWithVec = [copy](const double *x, const DVec &p)
    {
        return copy(x, p.data());
    };
    auto modelBind    = bind(modelWithVec, _1, par);

    return DoubleFunction(modelBind, getNArg());
}

DoubleFunction DoubleModel::toFunction(void) const
{
    DoubleModel copy(*this);

    auto func = [copy](const double *x){return copy(x, x + copy.getNArg());};

    return DoubleFunction(func, getNArg() + getNPar());
}
