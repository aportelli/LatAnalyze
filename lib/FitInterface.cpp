/*
 * FitInterface.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
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

#include <LatAnalyze/FitInterface.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

// error checks ////////////////////////////////////////////////////////////////
#define checkXDim(i)\
if ((i) >= getNXDim())\
{\
    LATAN_ERROR(Range, "X dimension " + strFrom(i) + " out of range");\
}

#define checkXIndex(vi, i)\
if ((vi) >= getXSize(i))\
{\
    LATAN_ERROR(Range, "index " + strFrom(vi) + " in X dimension "\
                + strFrom(i) + " out of range");\
}

#define checkYDim(j)\
if ((j) >= getNYDim())\
{\
    LATAN_ERROR(Range, "Y dimension " + strFrom(j) + " out of range");\
}

#define checkDataIndex(k)\
if ((k) >= maxDataIndex_)\
{\
    LATAN_ERROR(Range, "data point index " + strFrom(k) + " invalid");\
}

#define checkDataCoord(v)\
if (static_cast<Index>((v).size()) != getNXDim())\
{\
    LATAN_ERROR(Size, "number of coordinates and number of X dimensions "\
                "mismatch");\
}\
for (unsigned int i_ = 0; i_ < (v).size(); ++i_)\
{\
    checkXIndex((v)[i_], i_);\
}

#define checkPoint(k, j)\
if (!isXUsed(k, j))\
{\
    LATAN_ERROR(Range, "no data point in Y dimension " + strFrom(j)\
                + " with index " + strFrom(k));\
}

/******************************************************************************
 *                     FitInterface implementation                            *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
FitInterface::FitInterface(void)
{}

// add dimensions //////////////////////////////////////////////////////////////
void FitInterface::addXDim(const string name, const Index nData)
{
    if (getYSize() != 0)
    {
        LATAN_ERROR(Logic, "cannot add an X dimension if fit data is "
                    "not empty");
    }
    else
    {
        xDimName_.push_back(name);
        xSize_.push_back(nData);
        xDimIndex_[name]  = xDimName_.size();
        maxDataIndex_    *= nData;
        updateDataSize();
    }
}

void FitInterface::addYDim(const string name)
{
    yDimName_.push_back(name);
    yDataIndex_.push_back(map<Index, bool>());
    yDimIndex_[name] = yDimName_.size();
}

// size access /////////////////////////////////////////////////////////////////
Index FitInterface::getNXDim(void) const
{
    return xDimName_.size();
}

Index FitInterface::getNYDim(void) const
{
    return yDimName_.size();
}

Index FitInterface::getXSize(void) const
{
    Index size = 0;
    
    for (Index i = 0; i < getNXDim(); ++i)
    {
        size += getXSize(i);
    }
    
    return size;
}

Index FitInterface::getXSize(const Index i) const
{
    checkXDim(i);
    
    return xSize_[i];
}

Index FitInterface::getYSize(void) const
{
    Index size = 0;
    
    for (Index j = 0; j < getNYDim(); ++j)
    {
        size += getYSize(j);
    }
    
    return size;
}

Index FitInterface::getYSize(const Index j) const
{
    checkYDim(j);
    
    return static_cast<Index>(yDataIndex_[j].size());
}

Index FitInterface::getXFitSize(void) const
{
    Index size = 0;
    
    for (Index i = 0; i < getNXDim(); ++i)
    {
        size += getXFitSize(i);
    }
    
    return size;
}

Index FitInterface::getXFitSize(const Index i) const
{
    set<Index>    fitCoord;
    vector<Index> v;
    
    checkXDim(i);
    for (Index j = 0; j < getNYDim(); ++j)
    {
        for (auto &p: yDataIndex_[j])
        {
            if (p.second)
            {
                v = dataCoord(p.first);
                fitCoord.insert(v[i]);
            }
        }
    }
    
    return fitCoord.size();
}

Index FitInterface::getYFitSize(void) const
{
    Index size = 0;
    
    for (Index j = 0; j < getNYDim(); ++j)
    {
        size += getYFitSize(j);
    }
    
    return size;
}

Index FitInterface::getYFitSize(const Index j) const
{
    Index size;
    auto  pred = [](const pair<Index, bool> &p)
    {
        return p.second;
    };

    checkYDim(j);
    size = count_if(yDataIndex_[j].begin(), yDataIndex_[j].end(), pred);
    
    return size;
}

// Y dimension index helper ////////////////////////////////////////////////////
Index FitInterface::dataIndex(const vector<Index> &v) const
{
    Index k, n = static_cast<Index>(v.size());
    
    checkDataCoord(v);
    k = xSize_[1]*v[0];
    for (unsigned int d = 1; d < n-1; ++d)
    {
        k = xSize_[d+1]*(v[d] + k);
    }
    k += v[n-1];
    
    return k;
}

vector<Index> FitInterface::dataCoord(const Index k) const
{
    vector<Index> v(getNXDim());
    Index         buf, dimProd;
    
    checkDataIndex(k);
    buf     = k;
    dimProd = 1;
    for (Index d = getNXDim() - 1; d >= 0; --d)
    {
        v[d]     = (buf/dimProd)%xSize_[d];
        buf     -= dimProd*v[d];
        dimProd *= xSize_[d];
    }
    
    return v;
}

// enable fit points ///////////////////////////////////////////////////////////
void FitInterface::fitPoint(const bool isFitPoint, const Index k, const Index j)
{
    checkPoint(k, j);
    yDataIndex_[j][k] = isFitPoint;
}

// tests ///////////////////////////////////////////////////////////////////////
bool FitInterface::isXUsed(const Index k) const
{
    bool isUsed = false;
    
    for (Index j = 0; j < getNYDim(); ++j)
    {
        isUsed = isUsed or isXUsed(k, j);
    }
    
    return isUsed;
}

bool FitInterface::isXUsed(const Index k, const Index j) const
{
    checkYDim(j);
    
    return !(yDataIndex_[j].find(k) == yDataIndex_[j].end());
}

bool FitInterface::isFitPoint(const Index k, const Index j) const
{
    checkPoint(k, j);
    
    return yDataIndex_[j].at(k);
}

// register a data point ///////////////////////////////////////////////////////
void FitInterface::registerDataPoint(const Index k, const Index j)
{
    checkYDim(j);
    yDataIndex_[j][k] = true;
}

// IO //////////////////////////////////////////////////////////////////////////
ostream & Latan::operator<<(ostream &out, FitInterface &f)
{
    out << "X dimensions: " << f.getNXDim() << endl;
    for (Index i = 0; i < f.getNXDim(); ++i)
    {
        out << "  * " << i << " \"" << f.xDimName_[i] << "\": ";
        out << f.getXSize(i) << " value(s)" << endl;
    }
    out << "Y dimensions: " << f.getNYDim() << endl;
    for (Index j = 0; j < f.getNYDim(); ++j)
    {
        out << "  * " << j << " \"" << f.yDimName_[j] << "\": ";
        out << f.getYSize(j) << " value(s)" << endl;
        for (auto &p: f.yDataIndex_[j])
        {
            out << "    " << setw(3) << p.first << " (";
            for (auto vi: f.dataCoord(p.first))
            {
                out << vi << ",";
            }
            out << "\b) fit: " << (p.second ? "true" : "false") << endl;
        }
    }
    
    return out;
}
