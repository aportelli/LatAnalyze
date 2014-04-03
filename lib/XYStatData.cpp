/*
 * XYStatData.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/XYStatData.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                          FitResult implementation                          *
 ******************************************************************************/
// access //////////////////////////////////////////////////////////////////////
double FitResult::getChi2(void) const
{
    return chi2_;
}

Index FitResult::getNDof(void) const
{
    return nDof_;
}

double FitResult::getChi2PerDof(void) const
{
    return chi2_/static_cast<double>(nDof_);
}

const DoubleFunction & FitResult::getModel(const Index j) const
{
    return model_[static_cast<unsigned int>(j)];
}

/******************************************************************************
 *                         XYStatData implementation                          *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
XYStatData::XYStatData(void)
: chi2_(*this)
{}

XYStatData::XYStatData(const Index nData, const Index xDim, const Index yDim)
: XYStatData()
{
    resize(nData, xDim, yDim);
}

// access //////////////////////////////////////////////////////////////////////
void XYStatData::resize(const Index nData, const Index xDim, const Index yDim)
{
    FitInterface::resize(nData, xDim, yDim);
    x_.resize(nData, xDim);
    y_.resize(nData, yDim);
    var_[xx].resize(xDim, xDim);
    var_[yy].resize(yDim, yDim);
    var_[yx].resize(yDim, xDim);
    FOR_MAT(var_[xx], i1, i2)
    {
        var_[xx](i1, i2).resize(nData, nData);
    }
    FOR_MAT(var_[yy], j1, j2)
    {
        var_[yy](j1, j2).resize(nData, nData);
    }
    FOR_MAT(var_[yx], j, i)
    {
        var_[yx](j, i).resize(nData, nData);
    }
}

void XYStatData::reinitChi2(const bool doReinit)
{
    reinitChi2_ = doReinit;
}

Block<DMatBase> XYStatData::x(const PlaceHolder ph1 __unused,
                              const PlaceHolder ph2 __unused)
{
    return x_.block(0, 0, getNData(), getXDim());
}

ConstBlock<DMatBase> XYStatData::x(const PlaceHolder ph1 __unused,
                                   const PlaceHolder ph2 __unused) const
{
    return x_.block(0, 0, getNData(), getXDim());
}

Block<DMatBase> XYStatData::x(const Index i, const PlaceHolder ph2 __unused)
{
    return x_.block(0, i, getNData(), 1);
}

ConstBlock<DMatBase> XYStatData::x(const Index i,
                                   const PlaceHolder ph2 __unused) const
{
    return x_.block(0, i, getNData(), 1);
}

Block<DMatBase> XYStatData::x(const PlaceHolder ph1 __unused, const Index k)
{
    return x_.block(k, 0, 1, getXDim());
}

ConstBlock<DMatBase> XYStatData::x(const PlaceHolder ph1 __unused,
                                   const Index k) const
{
    return x_.block(k, 0, 1, getXDim());
}

double & XYStatData::x(const Index i, const Index k)
{
    return x_(k, i);
}

const double & XYStatData::x(const Index i, const Index k) const
{
    return x_(k, i);
}

Block<DMatBase> XYStatData::y(const PlaceHolder ph1 __unused,
                              const PlaceHolder ph2 __unused)
{
    return y_.block(0, 0, getNData(), getYDim());
}

ConstBlock<DMatBase> XYStatData::y(const PlaceHolder ph1 __unused,
                                   const PlaceHolder ph2 __unused) const
{
    return y_.block(0, 0, getNData(), getYDim());
}

Block<DMatBase> XYStatData::y(const Index j, const PlaceHolder ph2 __unused)
{
    return y_.block(0, j, getNData(), 1);
}

ConstBlock<DMatBase> XYStatData::y(const Index j,
                                   const PlaceHolder ph2 __unused) const
{
    return y_.block(0, j, getNData(), 1);
}

Block<DMatBase> XYStatData::y(const PlaceHolder ph1 __unused, const Index k)
{
    return y_.block(k, 0, 1, getYDim());
}

ConstBlock<DMatBase> XYStatData::y(const PlaceHolder ph1 __unused,
                                   const Index k) const
{
    return y_.block(k, 0, 1, getYDim());
}

double & XYStatData::y(const Index j, const Index k)
{
    return y_(k, j);
}

const double & XYStatData::y(const Index j, const Index k) const
{
    return y_(k, j);
}

#define FULL_BLOCK(m) (m).block(0, 0, (m).rows(), (m).cols())

Block<DMatBase> XYStatData::xxVar(const Index i1, const Index i2)
{
    return FULL_BLOCK(var_[xx](i1, i2));
}

ConstBlock<DMatBase> XYStatData::xxVar(const Index i1, const Index i2) const
{
    return FULL_BLOCK(var_[xx](i1, i2));
}

Block<DMatBase> XYStatData::yyVar(const Index j1, const Index j2)
{
    return FULL_BLOCK(var_[yy](j1, j2));
}

ConstBlock<DMatBase> XYStatData::yyVar(const Index j1, const Index j2) const
{
    return FULL_BLOCK(var_[yy](j1, j2));
}

Block<DMatBase> XYStatData::yxVar(const Index j, const Index i)
{
    return FULL_BLOCK(var_[yx](j, i));
}

ConstBlock<DMatBase> XYStatData::yxVar(const Index j, const Index i) const
{
    return FULL_BLOCK(var_[yx](j, i));
}

// fit /////////////////////////////////////////////////////////////////////////
FitResult XYStatData::fit(Minimizer &minimizer, const DVec &init,
                          const vector<const DoubleModel *> &modelVector)
{
    // initialization
    chi2_.setModel(modelVector);
    if (reinitChi2_)
    {
        chi2_.requestInit();
    }
    
    // initial parameters
    const Index nPoint = getNFitPoint();
    DVec        fullInit = init;
    Index       is = 0, kf = 0;
    
    fullInit.conservativeResize(chi2_.getNArg());
    for (Index i = 0; i < getXDim(); ++i)
    if (!isXExact(i))
    {
        for (Index k = 0; k < getNData(); ++k)
        if (isFitPoint(k))
        {
            fullInit(chi2_.getNPar() + nPoint*is + kf) = x(i, k);
            kf++;
        }
        is++;
    }
    minimizer.setInit(fullInit);
    
    // fit
    FitResult result;
    
    result        = minimizer(chi2_);
    result.chi2_  = chi2_(result);
    result.nDof_  = chi2_.getNDof();
    result.model_.resize(modelVector.size());
    for (unsigned int j = 0; j < modelVector.size(); ++j)
    {
        result.model_[j] = modelVector[j]->getBind(result);
    }
    
    return result;
}
