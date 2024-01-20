/*
 * DataFilter.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Physics/DataFilter.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Numerical/DWT.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                        DataFilter implementation                           *
 ******************************************************************************/
// constructor ////////////////////////////////////////////////////////////////
DataFilter::DataFilter(const vector<double> &filter, const bool downsample)
: filter_(filter), downsample_(downsample)
{}

// filtering //////////////////////////////////////////////////////////////////
template <typename MatType>
void filter(MatType &out, const MatType &in, const vector<double> &filter, 
            const bool downsample, MatType &buf)
{
    if (!downsample)
    {
        out.resizeLike(in);
        DWT::filterConvolution(out, in, filter, filter.size()/2);
    }
    else
    {
        out.resize(in.rows()/2, in.cols());
        buf.resizeLike(in);
        DWT::filterConvolution(buf, in, filter, filter.size()/2);
        DWT::downsample(out, buf);
    }
}

void DataFilter::operator()(DVec &out, const DVec &in)
{
    filter(out, in, filter_, downsample_, vBuf_);
}

void DataFilter::operator()(DMat &out, const DMat &in)
{
    filter(out, in, filter_, downsample_, mBuf_);
}

/******************************************************************************
 *                     LaplaceDataFilter implementation                       *
 ******************************************************************************/
// constructor ////////////////////////////////////////////////////////////////
LaplaceDataFilter::LaplaceDataFilter(const bool downsample)
: DataFilter({1., -2. , 1.}, downsample)
{}

// filtering //////////////////////////////////////////////////////////////////
void LaplaceDataFilter::operator()(DVec &out, const DVec &in, const double lambda)
{
    filter_[1] = -2. - lambda;
    DataFilter::operator()(out, in);
}

void LaplaceDataFilter::operator()(DMat &out, const DMat &in, const double lambda)
{
    filter_[1] = -2. - lambda;
    DataFilter::operator()(out, in);
}
