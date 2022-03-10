/*
 * DWT.hpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Latan_DWT_hpp_
#define Latan_DWT_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Numerical/DWTFilters.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                   Discrete wavelet transform class                         *
 ******************************************************************************/
class DWT
{
public:
  typedef std::pair<DVec, DVec> DWTLevel;
public:
    // constructor
    DWT(const DWTFilter &filter);
    // destructor
    virtual ~DWT(void) = default;
    // convolution primitive
    static void filterConvolution(DVec &out, const DVec &data, 
                                  const std::vector<double> &filter, const Index offset);
    // downsampling/upsampling primitives
    static void downsample(DVec &out, const DVec &in);
    static void upsample(DVec &out, const DVec &in);
    // DWT
    std::vector<DWTLevel> forward(const DVec &data, const unsigned int level) const;
    DVec                  backward(const std::vector<DWTLevel>& dwt) const;
private:
    DWTFilter filter_;
};

END_LATAN_NAMESPACE

#endif // Latan_DWT_hpp_
