/*
 * XYData.hpp, part of LatAnalyze 3
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

#ifndef Latan_XYData_hpp_
#define Latan_XYData_hpp_

#include <latan/Global.hpp>
#include <latan/Chi2Function.hpp>
#include <latan/Function.hpp>
#include <latan/Mat.hpp>
#include <latan/Minimizer.hpp>
#include <latan/Model.hpp>

BEGIN_NAMESPACE

/******************************************************************************
 *                           object for fit result                            *
 ******************************************************************************/
class FitResult: public DVec
{
    friend class XYStatData;
public:
    // constructors
    FitResult(void) = default;
    EIGEN_EXPR_CTOR(FitResult, FitResult, Base, MatrixBase)
    // destructor
    virtual ~FitResult(void) = default;
    // access
    double getChi2(void) const;
    double getChi2PerDof(void) const;
private:
    double chi2_{0.0};
    Index  nDof_{0};
};

/******************************************************************************
 *                    object for X vs. Y statistical data                     *
 ******************************************************************************/
const Index _ = -1;

class XYStatData
{
public:
    enum
    {
        xx = 0,
        yy = 1,
        yx = 2
    };
    typedef Minimizer::Verbosity FitVerbosity;
public:
    // constructors
    XYStatData(void);
    XYStatData(const Index nData, const Index nXDim, const Index nYDim);
    // destructor
    virtual ~XYStatData(void) = default;
    // access
    void                 assumeXExact(const Index i, const bool isExact = true);
    void                 fitPoint(const Index k, const bool isFitPoint = true);
    void                 fitPointRange(const Index k1, const Index k2,
                                       const bool isFitPoint = true);
    void                 fitAllPoints(const bool isFitPoint = true);
    Index                getNData(void)     const;
    Index                getNFitPoint(void) const;
    Index                getXDim(void)      const;
    Index                getYDim(void)      const;
    Index                getStatXDim(void)  const;
    void                 setNData(const Index nData);
    void                 setXDim(const Index xDim);
    void                 setYDim(const Index yDim);
    void                 resize(const Index nData, const Index xDim,
                                const Index yDim);
    Block<DMatBase>      x(const Index i = _, const Index k = _);
    ConstBlock<DMatBase> x(const Index i = _, const Index k = _) const;
    Block<DMatBase>      y(const Index j = _, const Index k = _);
    ConstBlock<DMatBase> y(const Index j = _, const Index k = _) const;
    Block<DMatBase>      xxVar(const Index i1, const Index i2);
    ConstBlock<DMatBase> xxVar(const Index i1, const Index i2) const;
    Block<DMatBase>      yyVar(const Index j1, const Index j2);
    ConstBlock<DMatBase> yyVar(const Index j1, const Index j2) const;
    Block<DMatBase>      yxVar(const Index j,  const Index i);
    ConstBlock<DMatBase> yxVar(const Index j,  const Index i) const;
    // test
    bool isFitPoint(const Index k) const;
    bool isXExact(const Index i) const;
    // fit
    FitResult fit(const std::vector<const DoubleModel *> &modelVector,
                  Minimizer &minimizer, const DVec &init,
                  const bool reinitChi2 = true,
                  const FitVerbosity verbosity = FitVerbosity::Silent);
    FitResult fit(const DoubleModel &model, Minimizer &minimizer,
                  const DVec &init, const bool reinitChi2 = true,
                  const FitVerbosity verbosity = FitVerbosity::Silent);
private:
    DMat         x_, y_;
    Mat<DMat>    var_[3];
    IVec         isXExact_, isFitPoint_;
    Chi2Function chi2_;
};

/******************************************************************************
 *                      XYStatData template implementation                    *
 ******************************************************************************/

END_NAMESPACE

#endif // Latan_XYData_hpp_
