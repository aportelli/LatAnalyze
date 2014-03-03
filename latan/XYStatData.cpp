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

#include <latan/XYStatData.hpp>
#include <latan/includes.hpp>

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

double FitResult::getChi2PerDof(void) const
{
    return chi2_/static_cast<double>(nDof_);
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
void XYStatData::assumeXExact(const Index i, const bool isExact)
{
    isXExact_[i] = (isExact) ? 1 : 0;
}

void XYStatData::fitPoint(const Index i, const bool isFitPoint)
{
    isFitPoint_[i] = (isFitPoint) ? 1 : 0;
}

void XYStatData::fitPointRange(const Index k1, const Index k2,
                               const bool isFitPoint)
{
    int size = static_cast<int>(k2-k1+1);
    
    isFitPoint_.segment(k1, size) = IVec::Constant(size, (isFitPoint) ? 1 : 0);
}

void XYStatData::fitAllPoints(const bool isFitPoint)
{
    fitPointRange(0, getNData()-1, isFitPoint);
}

Index XYStatData::getNData(void) const
{
    return x_.rows();
}

Index XYStatData::getNFitPoint(void) const
{
    return isFitPoint_.sum();
}

Index XYStatData::getXDim(void) const
{
    return x_.cols();
}

Index XYStatData::getYDim(void) const
{
    return y_.cols();
}

Index XYStatData::getStatXDim(void) const
{
    return isXExact_.size() - isXExact_.sum();
}

void XYStatData::setNData(const Index nData)
{
    resize(nData, getXDim(), getYDim());
}

void XYStatData::setXDim(const Index xDim)
{
    resize(getNData(), xDim, getYDim());
}

void XYStatData::setYDim(const Index yDim)
{
    resize(getNData(), getXDim(), yDim);
}

void XYStatData::resize(const Index nData, const Index xDim, const Index yDim)
{
    x_.conservativeResize(nData, xDim);
    y_.conservativeResize(nData, yDim);
    isXExact_.conservativeResize(xDim);
    isFitPoint_.conservativeResize(nData);
    var_[xx].conservativeResize(xDim, xDim);
    var_[yy].conservativeResize(yDim, yDim);
    var_[yx].conservativeResize(yDim, xDim);
    FOR_MAT(var_[xx], i1, i2)
    {
        var_[xx](i1, i2).conservativeResize(nData, nData);
    }
    FOR_MAT(var_[yy], j1, j2)
    {
        var_[yy](j1, j2).conservativeResize(nData, nData);
    }
    FOR_MAT(var_[yx], j, i)
    {
        var_[yx](j, i).conservativeResize(nData, nData);
    }
}

#define FULL_BLOCK(m) (m).block(0, 0, (m).rows(), (m).cols())
#define ACCESS_DATA(xy, ij, k) \
if ((ij >= 0)&&(k >= 0))\
{\
    return xy.block(k, ij, 1, 1);\
}\
else if ((ij < 0)&&(k >= 0))\
{\
    return xy.block(k, 0, 1, getXDim());\
}\
else if ((ij >= 0)&&(k < 0))\
{\
    return xy.block(0, ij, getNData(), 1);\
}\
else\
{\
    return xy.block(0, 0, getNData(), getXDim());\
}

Block<DMatBase> XYStatData::x(const Index i, const Index k)
{
    ACCESS_DATA(x_, i, k);
}

ConstBlock<DMatBase> XYStatData::x(const Index i, const Index k) const
{
    ACCESS_DATA(x_, i, k);
}

Block<DMatBase> XYStatData::y(const Index j, const Index k)
{
    ACCESS_DATA(y_, j, k);
}

ConstBlock<DMatBase> XYStatData::y(const Index j, const Index k) const
{
    ACCESS_DATA(y_, j, k);
}

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

// test ////////////////////////////////////////////////////////////////////////
bool XYStatData::isFitPoint(const Index k) const
{
    return (isFitPoint_[k] == 1);
}

bool XYStatData::isXExact(const Index i) const
{
    return (isXExact_[i] == 1);
}

// fit /////////////////////////////////////////////////////////////////////////
FitResult XYStatData::fit(const vector<const DoubleModel *> &modelVector,
                          Minimizer &minimizer, const DVec &init,
                          const bool reinitChi2,
                          const FitVerbosity verbosity)
{
    // initialization
    chi2_.setModel(modelVector);
    if (reinitChi2)
    {
        chi2_.requestInit();
    }
    minimizer.setVerbosity(verbosity);
    
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
            fullInit(chi2_.getNPar() + nPoint*is + kf) = x(i, k)(0, 0);
            kf++;
        }
        is++;
    }
    minimizer.setInit(fullInit);
    
    // fit
    FitResult result;
    
    result       = minimizer(chi2_);
    result.chi2_ = chi2_(result);
    result.nDof_ = chi2_.getNDof();
    
    return result;
}

FitResult XYStatData::fit(const DoubleModel &model, Minimizer &minimizer,
                          const DVec &init, const bool reinitChi2,
                          const FitVerbosity verbosity)
{
    vector<const DoubleModel *> modelVector(1);
    
    modelVector[0] = &model;
    
    return fit(modelVector, minimizer, init, reinitChi2, verbosity);
}
