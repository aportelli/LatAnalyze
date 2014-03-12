/*
 * XYSampleData.hpp, part of LatAnalyze 3
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

#ifndef Latan_XYSampleData_hpp_
#define Latan_XYSampleData_hpp_

#include <latan/Global.hpp>
#include <latan/FitInterface.hpp>
#include <latan/Function.hpp>
#include <latan/MatSample.hpp>
#include <latan/Minimizer.hpp>
#include <latan/XYStatData.hpp>

BEGIN_NAMESPACE

/******************************************************************************
 *                            object for fit result                           *
 ******************************************************************************/
class SampleFitResult: public DMatSample
{
    friend class XYSampleData;
public:
    // constructors
    using DMatSample::DMatSample;
    SampleFitResult(void) = default;
    // destructor
    virtual ~SampleFitResult(void) = default;
    // access
    double                       getChi2(const Index s = central) const;
    const DSample &              getChi2(const PlaceHolder ph) const;
    double                       getChi2PerDof(const Index s = central) const;
    DSample                      getChi2PerDof(const PlaceHolder ph) const;
    const DoubleFunction &       getModel(const Index s = central,
                                          const Index j = 0) const;
    const DoubleFunctionSample & getModel(const PlaceHolder ph,
                                          const Index j = 0) const;
private:
    DSample                           chi2_;
    Index                             nDof_{0};
    std::vector<DoubleFunctionSample> model_;
};

/******************************************************************************
 *                            XYSampleData                                    *
 ******************************************************************************/
class XYSampleData: public FitInterface
{
public:
    typedef DMatSample::Block      SampleBlock;
    typedef DMatSample::ConstBlock ConstSampleBlock;
public:
    // constructors
    XYSampleData(void) = default;
    XYSampleData(const Index nData, const Index xDim, const Index yDim,
                 const Index nSample);
    // destructor
    virtual ~XYSampleData(void) = default;
    // access
    const XYStatData & getData(const Index s = central);
    void             resize(const Index nData, const Index xDim,
                            const Index yDim, const Index nSample);
    SampleBlock      x(const PlaceHolder ph1 = _, const PlaceHolder ph2 = _);
    ConstSampleBlock x(const PlaceHolder ph1 = _,
                       const PlaceHolder ph2 = _) const;
    SampleBlock      x(const Index i, const PlaceHolder ph2 = _);
    ConstSampleBlock x(const Index i, const PlaceHolder ph2 = _) const;
    SampleBlock      x(const PlaceHolder ph1, const Index k);
    ConstSampleBlock x(const PlaceHolder ph1, const Index k) const;
    SampleBlock      x(const Index i, const Index k);
    ConstSampleBlock x(const Index i, const Index k) const;
    SampleBlock      y(const PlaceHolder ph1 = _, const PlaceHolder ph2 = _);
    ConstSampleBlock y(const PlaceHolder ph1 = _,
                       const PlaceHolder ph2 = _) const;
    SampleBlock      y(const Index i, const PlaceHolder ph2 = _);
    ConstSampleBlock y(const Index i, const PlaceHolder ph2 = _) const;
    SampleBlock      y(const PlaceHolder ph1, const Index k);
    ConstSampleBlock y(const PlaceHolder ph1, const Index k) const;
    SampleBlock      y(const Index i, const Index k);
    ConstSampleBlock y(const Index i, const Index k) const;
    // fit
    SampleFitResult fit(const std::vector<const DoubleModel *> &modelVector,
                        Minimizer &minimizer, const DVec &init,
                        const FitVerbosity verbosity = FitVerbosity::Silent);
    SampleFitResult fit(const DoubleModel &model, Minimizer &minimizer,
                        const DVec &init, 
                        const FitVerbosity verbosity = FitVerbosity::Silent);
private:
    void setDataToSample(const Index s);
private:
    bool isCovarianceInit_;
    DMatSample x_, y_;
    XYStatData data_;
};

END_NAMESPACE

#endif // Latan_XYSampleData_hpp_
