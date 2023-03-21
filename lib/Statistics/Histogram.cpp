/*
 * Histogram.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#include <LatAnalyze/Statistics/Histogram.hpp>
#include <LatAnalyze/includes.hpp>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_sort.h>

using namespace std;
using namespace Latan;

#define DECL_GSL_HIST(h) \
gsl_histogram h{static_cast<size_t>(bin_.size()), x_.data(), bin_.data()}
#define DECL_CONST_GSL_HIST(h) \
const gsl_histogram h{static_cast<size_t>(bin_.size()),\
                      const_cast<double *>(x_.data()),\
                      const_cast<double *>(bin_.data())}

/******************************************************************************
 *                         Histogram implementation                           *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
Histogram::Histogram(const DVec &data, const double xMin, const double xMax,
                     const Index nBin)
: Histogram()
{
    setFromData(data, xMin, xMax, nBin);
}

Histogram::Histogram(const DVec &data, const DVec &w, const double xMin,
                     const double xMax, const Index nBin)
: Histogram()
{
    setFromData(data, w, xMin, xMax, nBin);
}

// resize //////////////////////////////////////////////////////////////////////
void Histogram::resize(const Index nBin)
{
    x_.resize(nBin + 1);
    bin_.resize(nBin);
}

// generate from data //////////////////////////////////////////////////////////
void Histogram::setFromData(const DVec &data, const DVec &w, const double xMin,
                            const double xMax, const Index nBin)
{
    if (data.size() != w.size())
    {
        LATAN_ERROR(Size, "data vector and weight vector size mismatch");
    }
    resize(nBin);
    data_ = data.array();
    w_    = w.array();
    xMax_ = xMax;
    xMin_ = xMin;
    makeHistogram();
}

void Histogram::setFromData(const DVec &data, const double xMin,
                            const double xMax, const Index nBin)
{
    resize(nBin);
    data_ = data.array();
    xMax_ = xMax;
    xMin_ = xMin;
    w_.resize(data.size());
    w_.fill(1.);
    makeHistogram();
}

// histogram calculation ///////////////////////////////////////////////////////
void Histogram::makeHistogram(void)
{
    DECL_GSL_HIST(h);
    
    gsl_histogram_set_ranges_uniform(&h, xMin_, xMax_);
    FOR_STAT_ARRAY(data_, i)
    {
        gsl_histogram_accumulate(&h, data_[i], w_[i]);
    }
    total_ = w_.sum();
    sortIndices();
    computeNorm();
}

// generate sorted indices /////////////////////////////////////////////////////
void Histogram::sortIndices(void)
{
    sInd_.resize(data_.size());
    gsl_sort_index(sInd_.data(), data_.data(), 1, data_.size());
}

// compute normalization factor ////////////////////////////////////////////////
void Histogram::computeNorm(void)
{
    norm_ = static_cast<double>(bin_.size())/(total_*(xMax_ - xMin_));
}

// normalize as a probablility /////////////////////////////////////////////////
void Histogram::normalize(const bool n)
{
    normalize_ = n;
}

bool Histogram::isNormalized(void) const
{
    return normalize_;
}

// access //////////////////////////////////////////////////////////////////////
Index Histogram::size(void) const
{
    return bin_.size();
}

const StatArray<double> & Histogram::getData(void) const
{
    return data_;
}

const StatArray<double> & Histogram::getWeight(void) const
{
    return w_;
}

double Histogram::getX(const Index i) const
{
    return x_(i);
}

double Histogram::operator[](const Index i) const
{
    return bin_(i)*(isNormalized() ? norm_ : 1.);
}

double Histogram::operator()(const double x) const
{
    size_t i;

    DECL_CONST_GSL_HIST(h);

    gsl_histogram_find(&h, x, &i);

    return (*this)[static_cast<Index>(i)];
}

double Histogram::getMax(void) const
{
    return bin_.maxCoeff()*(isNormalized() ? norm_ : 1.);
}


// percentiles & confidence interval ///////////////////////////////////////////
double Histogram::percentile(const double p) const
{
    if ((p < 0.0) or (p > 100.0))
    {
        LATAN_ERROR(Range, "percentile (" + strFrom(p) + ")"
                    " is outside the [0, 100] range");
    }
    
    // cf. http://en.wikipedia.org/wiki/Percentile
    double wPSum, p_i, p_im1, w_i, res = 0.;
    bool   haveResult;
    
    wPSum = w_[sInd_[0]];
    p_i   = (100./total_)*wPSum*0.5;
    if (p < p_i)
    {
        res = data_[sInd_[0]];
    }
    else
    {
        haveResult = false;
        p_im1      = p_i;
        for (Index i = 1; i < data_.size(); ++i)
        {
            w_i    = w_[sInd_[i]];
            wPSum += w_i;
            p_i    = (100./total_)*(wPSum-0.5*w_i);
            if ((p >= p_im1) and (p < p_i))
            {
                double d_i = data_[sInd_[i]], d_im1 = data_[sInd_[i-1]];

                res        = d_im1 + (p-p_im1)/(p_i-p_im1)*(d_i-d_im1);
                haveResult = true;
                break;
            }
        }
        if (!haveResult)
        {
            res = data_[sInd_[data_.size()-1]];
        }
    }
    
    return res;
}

double Histogram::median(void) const
{
    return percentile(50.);
}

pair<double, double> Histogram::confidenceInterval(const double nSigma) const
{
    pair<double, double> interval, p;
    double               cl;
    
    cl              = gsl_sf_erf(nSigma/sqrt(2.));
    p.first         = 50.*(1. - cl);
    p.second        = 50.*(1. + cl);
    interval.first  = percentile(p.first);
    interval.second = percentile(p.second);
    
    return interval;
}
