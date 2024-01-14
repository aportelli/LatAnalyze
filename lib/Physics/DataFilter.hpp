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
#include <LatAnalyze/Statistics/MatSample.hpp>

BEGIN_LATAN_NAMESPACE

class DataFilter
{
public:
    // constructor
    DataFilter(const std::vector<double> &filter, const bool downsample = false);
    // filtering
    virtual void operator()(DVec &out, const DVec &in);
    virtual void operator()(DMat &out, const DMat &in);
    virtual void operator()(DMatSample &out, const DMatSample &in);
protected:
    std::vector<double> filter_;
private:
    bool                downsample_;
    DVec                vBuf_;
    DMat                mBuf_;
};

class LaplaceDataFilter: public DataFilter
{
public:
    // constructor
    LaplaceDataFilter(const bool downsample = false);
    // filtering
    virtual void operator()(DVec &out, const DVec &in, const double lambda = 0.);
    virtual void operator()(DMat &out, const DMat &in, const double lambda = 0.);
    virtual void operator()(DMatSample &out, const DMatSample &in, const double lambda = 0.);
};

END_LATAN_NAMESPACE

#endif // Latan_DataFilter_hpp_
