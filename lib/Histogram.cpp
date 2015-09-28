/*
 * Histogram.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Histogram.hpp>
#include <LatAnalyze/includes.hpp>

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

    DECL_GSL_HIST(h);

    xMax_ = xMax;
    xMin_ = xMin;
    gsl_histogram_set_ranges_uniform(&h, xMin, xMax);
    FOR_VEC(data, i)
    {
        gsl_histogram_accumulate(&h, data(i), w(i));
    }
    total_ = w.sum();
    computeNorm();
}

void Histogram::setFromData(const DVec &data, const double xMin,
                            const double xMax, const Index nBin)
{
    resize(nBin);

    DECL_GSL_HIST(h);

    xMax_ = xMax;
    xMin_ = xMin;
    gsl_histogram_set_ranges_uniform(&h, xMin, xMax);
    FOR_VEC(data, i)
    {
        gsl_histogram_increment(&h, data(i));
    }
    total_ = static_cast<double>(data.size());
    computeNorm();
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
