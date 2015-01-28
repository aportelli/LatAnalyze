/*
 * FitInterface.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/FitInterface.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                       FitInterface implementation                          *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
FitInterface::FitInterface(const Index nData, const Index xDim,
                           const Index yDim)
{
    resize(nData, xDim, yDim);
}

// access //////////////////////////////////////////////////////////////////////
void FitInterface::assumeXExact(const Index i, const bool isExact)
{
    isXExact_(i) = (isExact) ? 1 : 0;
}

void  FitInterface::assumeXXCorrelated(const Index i1, const Index i2,
                                       const bool isCorrelated)
{
    int val = (isCorrelated) ? 1 : 0;
    
    isXXCorr_(i1, i2) = val;
    isXXCorr_(i2, i1) = val;
}

void  FitInterface::assumeYYCorrelated(const Index j1, const Index j2,
                                       const bool isCorrelated)
{
    int val = (isCorrelated) ? 1 : 0;
    
    isYYCorr_(j1, j2) = val;
    isYYCorr_(j2, j1) = val;
}

void  FitInterface::assumeYXCorrelated(const Index j, const Index i,
                                       const bool isCorrelated)
{
    int val = (isCorrelated) ? 1 : 0;
    
    isYXCorr_(j, i) = val;
}

void  FitInterface::assumeDataCorrelated(const Index k1, const Index k2,
                                         const bool isCorrelated)
{
    int val = (isCorrelated) ? 1 : 0;
    
    isDataCorr_(k1, k2) = val;
    isDataCorr_(k2, k1) = val;
}

void  FitInterface::assumeDataCorrelated(const bool isCorrelated)
{
    FOR_MAT(isDataCorr_, k1, k2)
    {
        if (k1 != k2)
        {
            assumeDataCorrelated(k1, k2, isCorrelated);
        }
    }
}

void FitInterface::fitPoint(const Index i, const bool isFitPoint)
{
    isFitPoint_(i) = (isFitPoint) ? 1 : 0;
}

void FitInterface::fitPointRange(const Index k1, const Index k2,
                                 const bool isFitPoint)
{
    int size = static_cast<int>(k2-k1+1);
    
    isFitPoint_.segment(k1, size) = IVec::Constant(size, (isFitPoint) ? 1 : 0);
}

void FitInterface::fitAllPoints(const bool isFitPoint)
{
    fitPointRange(0, getNData()-1, isFitPoint);
}

Index FitInterface::getNData(void) const
{
    return isFitPoint_.size();
}

Index FitInterface::getNFitPoint(void) const
{
    return isFitPoint_.sum();
}

Index FitInterface::getXDim(void) const
{
    return isXXCorr_.rows();
}

Index FitInterface::getYDim(void) const
{
    return isYYCorr_.rows();
}

Index FitInterface::getStatXDim(void) const
{
    return isXExact_.size() - isXExact_.sum();
}

void FitInterface::setFitInterface(const FitInterface &fitInterface)
{
    if (&fitInterface != this)
    {
        isXExact_   = fitInterface.isXExact_;
        isFitPoint_ = fitInterface.isFitPoint_;
        isXXCorr_   = fitInterface.isXXCorr_;
        isYYCorr_   = fitInterface.isYYCorr_;
        isYXCorr_   = fitInterface.isYXCorr_;
        isDataCorr_ = fitInterface.isDataCorr_;
    }
}

void FitInterface::setNData(const Index nData)
{
    resize(nData, getXDim(), getYDim());
}

void FitInterface::setXDim(const Index xDim)
{
    resize(getNData(), xDim, getYDim());
}

void FitInterface::setYDim(const Index yDim)
{
    resize(getNData(), getXDim(), yDim);
}

void FitInterface::resize(const Index nData, const Index xDim, const Index yDim)
{
    isXExact_.setConstant(xDim, 0);
    isFitPoint_.setConstant(nData, 0);
    isXXCorr_.setIdentity(xDim, xDim);
    isYYCorr_.setIdentity(yDim, yDim);
    isYXCorr_.setConstant(yDim, xDim, 0);
    isDataCorr_.setIdentity(nData, nData);
}

// test ////////////////////////////////////////////////////////////////////////
bool FitInterface::isFitPoint(const Index k) const
{
    return (isFitPoint_[k] == 1);
}

bool FitInterface::isXExact(const Index i) const
{
    return (isXExact_[i] == 1);
}

bool FitInterface::isXXCorrelated(const Index i1, const Index i2) const
{
    return (isXXCorr_(i1, i2) == 1);
}

bool FitInterface::isYYCorrelated(const Index j1, const Index j2) const
{
    return (isYYCorr_(j1, j2) == 1);
}

bool FitInterface::isYXCorrelated(const Index j, const Index i) const
{
    return (isYXCorr_(j, i) == 1);
}

bool FitInterface::isDataCorrelated(const Index k1, const Index k2) const
{
    return (isDataCorr_(k1, k2) == 1);
}
