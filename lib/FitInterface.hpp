/*
 * FitInterface.hpp, part of LatAnalyze 3
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

#ifndef Latan_FitInterface_hpp_
#define Latan_FitInterface_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Minimizer.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                         base class for data fit                            *
 ******************************************************************************/
class FitInterface
{
public:
    typedef Minimizer::Verbosity FitVerbosity;
public:
    // constructors
    FitInterface(void) = default;
    FitInterface(const Index nData, const Index xDim, const Index yDim);
    // destructor
    virtual ~FitInterface(void) = default;
    // access
    void  assumeXExact(const Index i, const bool isExact = true);
    void  assumeXXCorrelated(const Index i1, const Index i2,
                             const bool isCorrelated = true);
    void  assumeYYCorrelated(const Index j1, const Index j2,
                             const bool isCorrelated = true);
    void  assumeYXCorrelated(const Index j, const Index i,
                             const bool isCorrelated = true);
    void  assumeDataCorrelated(const Index k1, const Index k2,
                               const bool isCorrelated = true);
    void  assumeDataCorrelated(const bool isCorrelated = true);
    void  fitPoint(const Index k, const bool isFitPoint = true);
    void  fitPointRange(const Index k1, const Index k2,
                        const bool isFitPoint = true);
    void  fitAllPoints(const bool isFitPoint = true);
    Index getNData(void)     const;
    Index getNFitPoint(void) const;
    Index getXDim(void)      const;
    Index getYDim(void)      const;
    Index getStatXDim(void)  const;
    void  setFitInterface(const FitInterface &fitInterface);
    void  setNData(const Index nData);
    void  setXDim(const Index xDim);
    void  setYDim(const Index yDim);
    void  resize(const Index nData, const Index xDim, const Index yDim);
    // test
    bool isFitPoint(const Index k) const;
    bool isXExact(const Index i) const;
    bool isXXCorrelated(const Index i1, const Index i2) const;
    bool isYYCorrelated(const Index j1, const Index j2) const;
    bool isYXCorrelated(const Index j, const Index i) const;
    bool isDataCorrelated(const Index k1, const Index k2) const;
private:
    IVec isXExact_, isFitPoint_;
    IMat isXXCorr_, isYYCorr_, isYXCorr_, isDataCorr_;
};

END_LATAN_NAMESPACE

#endif // Latan_FitInterface_hpp_
