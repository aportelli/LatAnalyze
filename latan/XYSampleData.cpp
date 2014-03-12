/*
 * XYSampleData.cpp, part of LatAnalyze 3
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

#include <latan/XYSampleData.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                      SampleFitResult implementation                        *
 ******************************************************************************/
double SampleFitResult::getChi2(const Index s) const
{
    return chi2_[s];
}

const DSample & SampleFitResult::getChi2(const PlaceHolder ph __unused) const
{
    return chi2_;
}

double SampleFitResult::getChi2PerDof(const Index s) const
{
    return chi2_[s]/static_cast<double>(nDof_);
}

DSample SampleFitResult::getChi2PerDof(const PlaceHolder ph __unused) const
{
    return chi2_/static_cast<double>(nDof_);
}
const DoubleFunction & SampleFitResult::getModel(const Index s, 
                                                 const Index j) const
{
    return model_[static_cast<unsigned int>(j)][s];
}
                                    
const DoubleFunctionSample & SampleFitResult::getModel(
                                                const PlaceHolder ph __unused,
                                                const Index j) const
{
    return model_[static_cast<unsigned int>(j)];
}

/******************************************************************************
 *                       XYSampleData implementation                          *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
XYSampleData::XYSampleData(const Index nData, const Index xDim, 
                           const Index yDim, const Index nSample)
{
    resize(nData, xDim, yDim, nSample);
}

// access //////////////////////////////////////////////////////////////////////
const XYStatData & XYSampleData::getData(const Index s)
{
    setDataToSample(s);
    
    return data_;
}

void XYSampleData::resize(const Index nData, const Index xDim,
                          const Index yDim, const Index nSample)
{
    FitInterface::resize(nData, xDim, yDim);
    x_.resize(nSample);
    x_.resizeMat(nData, xDim);
    y_.resize(nSample);
    y_.resizeMat(nData, yDim);
    data_.resize(nData, xDim, yDim);
    isCovarianceInit_ = false;
}

XYSampleData::SampleBlock XYSampleData::x(const PlaceHolder ph1 __unused,
                                          const PlaceHolder ph2 __unused)
{
    isCovarianceInit_ = false;
    
    return x_.block(0, 0, getNData(), getXDim());
}

XYSampleData::ConstSampleBlock XYSampleData::x(const PlaceHolder ph1 __unused,
                                               const PlaceHolder ph2 __unused)
                                               const
{
    return x_.block(0, 0, getNData(), getXDim());
}

XYSampleData::SampleBlock XYSampleData::x(const Index i, 
                                          const PlaceHolder ph2 __unused)
{
    isCovarianceInit_ = false;
    
    return x_.block(0, i, getNData(), 1);
}
                                          
XYSampleData::ConstSampleBlock XYSampleData::x(const Index i,
                                               const PlaceHolder ph2 __unused)
                                               const
{
    return x_.block(0, i, getNData(), 1);
}

XYSampleData::SampleBlock XYSampleData::x(const PlaceHolder ph1 __unused,
                                          const Index k)
{
    isCovarianceInit_ = false;
    
    return x_.block(k, 0, 1, getXDim());
}

XYSampleData::ConstSampleBlock XYSampleData::x(const PlaceHolder ph1 __unused,
                                               const Index k) const
{
    return x_.block(k, 0, 1, getXDim());
}

XYSampleData::SampleBlock XYSampleData::x(const Index i, const Index k)
{
    isCovarianceInit_ = false;
    
    return x_.block(k, i, 1, 1);
}

XYSampleData::ConstSampleBlock XYSampleData::x(const Index i, const Index k)   
                                               const
{
    return x_.block(k, i, 1, 1);
}

XYSampleData::SampleBlock XYSampleData::y(const PlaceHolder ph1 __unused,
                                          const PlaceHolder ph2 __unused)
{
    isCovarianceInit_ = false;
    
    return y_.block(0, 0, getNData(), getXDim());
}

XYSampleData::ConstSampleBlock XYSampleData::y(const PlaceHolder ph1 __unused,
                                               const PlaceHolder ph2 __unused)
                                               const
{
    return y_.block(0, 0, getNData(), getXDim());
}

XYSampleData::SampleBlock XYSampleData::y(const Index j, 
                                          const PlaceHolder ph2 __unused)
{
    isCovarianceInit_ = false;
    
    return y_.block(0, j, getNData(), 1);
}
                                          
XYSampleData::ConstSampleBlock XYSampleData::y(const Index j,
                                               const PlaceHolder ph2 __unused)
                                               const
{
    return y_.block(0, j, getNData(), 1);
}

XYSampleData::SampleBlock XYSampleData::y(const PlaceHolder ph1 __unused,
                                          const Index k)
{
    isCovarianceInit_ = false;
    
    return y_.block(k, 0, 1, getXDim());
}

XYSampleData::ConstSampleBlock XYSampleData::y(const PlaceHolder ph1 __unused,
                                               const Index k) const
{
    return y_.block(k, 0, 1, getXDim());
}

XYSampleData::SampleBlock XYSampleData::y(const Index j, const Index k)
{
    isCovarianceInit_ = false;
    
    return y_.block(k, j, 1, 1);
}

XYSampleData::ConstSampleBlock XYSampleData::y(const Index j, const Index k)   
                                               const
{
    return y_.block(k, j, 1, 1);
}

// fit /////////////////////////////////////////////////////////////////////////
SampleFitResult XYSampleData::fit(
                    const std::vector<const DoubleModel *> &modelVector,
                    Minimizer &minimizer, const DVec &init,
                    const FitVerbosity verbosity)
{
    const Index     nSample = x_.size();
    FitResult       sampleResult;
    SampleFitResult result;
    bool            initChi2;
    
    // copy interface to sample data
    data_.setFitInterface(*this);
    
    // sample loop
    result.resize(nSample);
    result.chi2_.resize(nSample);
    FOR_STAT_ARRAY(x_, s)
    {
        // set data
        setDataToSample(s);
        
        // initialize chi^2 only once
        initChi2 = (s == central);
        
        // fit
        sampleResult = data_.fit(modelVector, minimizer, init, initChi2,
                                 verbosity);
        
        // store result
        result[s]       = sampleResult;
        result.chi2_[s] = sampleResult.getChi2();
        result.nDof_    = sampleResult.getNDof();
        result.model_.resize(modelVector.size());
        for (unsigned int j = 0; j < modelVector.size(); ++j)
        {
            result.model_[j].resize(nSample);
            result.model_[j][s] = sampleResult.getModel(j);
        }
    }
    
    return result;
}

SampleFitResult XYSampleData::fit(const DoubleModel &model,
                                  Minimizer &minimizer,
                                  const DVec &init,
                                  const FitVerbosity verbosity)
{
    vector<const DoubleModel *> modelVector(1);
    
    modelVector[0] = &model;
    
    return fit(modelVector, minimizer, init, verbosity);
}

void XYSampleData::setDataToSample(const Index s)
{
    // compute covariance matrices if necessary
    if (!isCovarianceInit_)
    {
        DMatSample buf1, buf2;
            
        for (Index i2 = 0; i2 < getXDim(); ++i2)
        for (Index i1 = 0; i1 < getXDim(); ++i1)
        {
            buf1 = x(i1);
            buf2 = x(i2);
            data_.xxVar(i1, i2) = buf1.covarianceMatrix(buf2);
        }
        for (Index j2 = 0; j2 < getYDim(); ++j2)
        for (Index j1 = 0; j1 < getYDim(); ++j1)
        {
            buf1 = y(j1);
            buf2 = y(j2);
            data_.yyVar(j1, j2) = buf1.covarianceMatrix(buf2);
        }
        for (Index i = 0; i < getXDim(); ++i)
        for (Index j = 0; j < getYDim(); ++j)
        {
            buf1 = y(j);
            buf2 = x(i);
            data_.yxVar(j, i) = buf1.covarianceMatrix(buf2);
        }
        isCovarianceInit_ = true;
    }
    // set data
    data_.x() = x_[s];
    data_.y() = y_[s];
}
