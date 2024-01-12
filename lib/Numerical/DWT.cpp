/*
 * DWT.cpp, part of LatAnalyze
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

#include <LatAnalyze/Numerical/DWT.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                            DWT implementation                              *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
DWT::DWT(const DWTFilter &filter)
: filter_(filter)
{}

// convolution primitive ///////////////////////////////////////////////////////
void DWT::filterConvolution(DVec &out, const DVec &data, 
                            const std::vector<double> &filter, const Index offset)
{
    Index n = data.size(), nf = n*filter.size();

    out.resize(n);
    out.fill(0.);
    for (unsigned int i = 0; i < filter.size(); ++i)
    {
        FOR_VEC(out, j)
        {
            out(j) += filter[i]*data((j + i + nf - offset) % n);
        }
    }
}

// downsampling/upsampling primitives //////////////////////////////////////////
void DWT::downsample(DVec &out, const DVec &in)
{
    if (out.size() < in.size()/2)
    {
        LATAN_ERROR(Size, "output vector smaller than half the input vector size");
    }
    for (Index i = 0; i < in.size(); i += 2)
    {
        out(i/2) = in(i);
    }
}

void DWT::upsample(DVec &out, const DVec &in)
{
    if (out.size() < 2*in.size())
    {
        LATAN_ERROR(Size, "output vector smaller than twice the input vector size");
    }
    out.segment(0, 2*in.size()).fill(0.);
    for (Index i = 0; i < in.size(); i ++)
    {
        out(2*i) = in(i);
    }
}

// DWT /////////////////////////////////////////////////////////////////////////
std::vector<DWT::DWTLevel> 
DWT::forward(const DVec &data, const unsigned int level) const
{
    std::vector<DWTLevel> dwt(level);
    DVec                  *finePt = const_cast<DVec *>(&data);
    DVec                  tmp; 
    Index                 n = data.size(), o = filter_.fwdL.size()/2, minSize;

    minSize = 1;
    for (unsigned int l = 0; l < level; ++l) minSize *= 2;
    if (n < minSize)
    {
        LATAN_ERROR(Size, "data vector too small for a " + strFrom(level) 
                          + "-level DWT (data size is " + strFrom(n) + ")");
    }
    for (unsigned int l = 0; l < level; ++l)
    {
        n /= 2;
        dwt[l].first.resize(n);
        dwt[l].second.resize(n);
        filterConvolution(tmp, *finePt, filter_.fwdL, o);
        downsample(dwt[l].first, tmp);
        filterConvolution(tmp, *finePt, filter_.fwdH, o);
        downsample(dwt[l].second, tmp);
        finePt = &dwt[l].first;
    }

    return dwt;
}

DVec DWT::backward(const std::vector<DWTLevel>& dwt) const
{
    unsigned int level = dwt.size();
    Index        n = dwt.back().second.size(), o = filter_.bwdL.size()/2 - 1;
    DVec         res, tmp, conv;

    res = dwt.back().first;
    for (int l = level - 2; l >= 0; --l)
    {
        n *= 2;
        if (dwt[l].second.size() != n)
        {
            LATAN_ERROR(Size, "DWT result size mismatch");
        }
    }
    n = dwt.back().second.size();
    for (int l = level - 1; l >= 0; --l)
    {
        n *= 2;
        tmp.resize(n);
        upsample(tmp, res);
        filterConvolution(conv, tmp, filter_.bwdL, o);
        res = conv;
        upsample(tmp, dwt[l].second);
        filterConvolution(conv, tmp, filter_.bwdH, o);
        res += conv;
    }

    return res;
}

// concatenate levels //////////////////////////////////////////////////////////
DVec DWT::concat(const std::vector<DWTLevel> &dwt, const int maxLevel, const bool dropLow)
{
    unsigned int level = ((maxLevel >= 0) ? (maxLevel + 1) : dwt.size());
    Index nlast = dwt[level - 1].first.size();
    Index n = 2*dwt.front().first.size() - ((dropLow) ? nlast : 0);
    Index pt = n, nl;
    DVec  res(n);

    for (unsigned int l = 0; l < level; ++l)
    {
        nl  = dwt[l].second.size();
        pt -= nl;
        res.segment(pt, nl) = dwt[l].second;
    }
    if (!dropLow)
    {
        res.segment(0, nl) = dwt[level-1].first;
    }

    return res;
}
