/*
 * DataFilter.hpp, part of LatAnalyze 3
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

#ifndef Latan_DataFilter_hpp_
#define Latan_DataFilter_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Core/Math.hpp>
#include <LatAnalyze/Statistics/StatArray.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>
#include <LatAnalyze/Numerical/Minimizer.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                    Generic convolution filter class                        *
 ******************************************************************************/
class DataFilter
{
public:
    // constructor
    DataFilter(const std::vector<double> &filter, const bool downsample = false);
    // filtering
    void operator()(DVec &out, const DVec &in);
    void operator()(DMat &out, const DMat &in);
    template <typename MatType, Index o>
    void operator()(StatArray<MatType, o> &out, const StatArray<MatType, o> &in);
protected:
    std::vector<double> filter_;
private:
    bool                downsample_;
    DVec                vBuf_;
    DMat                mBuf_;
};

/******************************************************************************
 *                         Laplacian filter class                             *
 ******************************************************************************/
class LaplaceDataFilter: public DataFilter
{
public:
    // constructor
    LaplaceDataFilter(const bool downsample = false);
    // filtering
    void operator()(DVec &out, const DVec &in, const double lambda = 0.);
    void operator()(DMat &out, const DMat &in, const double lambda = 0.);
    template <typename MatType, Index o>
    void operator()(StatArray<MatType, o> &out, const StatArray<MatType, o> &in, 
                    const double lambda = 0.);
    // correlation optimisation
    template <typename MatType, Index o>
    double optimiseCdr(const StatArray<MatType, o> &data, Minimizer &min,
                       const unsigned int nPass = 3);
};

/******************************************************************************
 *                 DataFilter class template implementation                   *
 ******************************************************************************/
// filtering //////////////////////////////////////////////////////////////////
template <typename MatType, Index o>
void DataFilter::operator()(StatArray<MatType, o> &out, const StatArray<MatType, o> &in)
{
    FOR_STAT_ARRAY(in, s)
    {
        (*this)(out[s], in[s]);
    }
}

/******************************************************************************
 *            LaplaceDataFilter class template implementation                 *
 ******************************************************************************/
// filtering //////////////////////////////////////////////////////////////////
template <typename MatType, Index o>
void LaplaceDataFilter::operator()(StatArray<MatType, o> &out, 
                                   const StatArray<MatType, o> &in, const double lambda)
{
    FOR_STAT_ARRAY(in, s)
    {
        (*this)(out[s], in[s], lambda);
    }
}

// correlation optimisation ///////////////////////////////////////////////////
template <typename MatType, Index o>
double LaplaceDataFilter::optimiseCdr(const StatArray<MatType, o> &data, 
                                      Minimizer &min, const unsigned int nPass)
{
    StatArray<MatType, o> fdata(data.size());
    DVec init(1);
    double reg, prec;
    DoubleFunction cdr([&data, &fdata, this](const double *x)
    {
        double res;
        (*this)(fdata, data, x[0]);
        res = Math::cdr(fdata.correlationMatrix());
        
        return res;
    }, 1);

    min.setLowLimit(0., -0.1);
    min.setHighLimit(0., 100000.);
    init(0) = 0.1;
    min.setInit(init);
    prec = 0.1;
    min.setPrecision(prec);
    reg = min(cdr)(0);
    for (unsigned int pass = 0; pass < nPass; pass++)
    {
        min.setLowLimit(0., (1.-10.*prec)*reg);
        min.setHighLimit(0., (1.+10.*prec)*reg);
        init(0) = reg;
        min.setInit(init);
        prec *= 0.1;
        min.setPrecision(prec);
        reg = min(cdr)(0);
    }

    return reg;
}

END_LATAN_NAMESPACE

#endif // Latan_DataFilter_hpp_
