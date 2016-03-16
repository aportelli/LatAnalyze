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

/******************************************************************************
 *                     FitInterface implementation                            *
 ******************************************************************************/
// copy object (not as a constructor to be accessed from derived class) ////////
void FitInterface::copyInterface(const FitInterface &d)
{
    *this = d;
}

// add dimensions //////////////////////////////////////////////////////////////
void FitInterface::addXDim(const string name, const Index nData,
                           const bool isExact)
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
        xIsExact_.push_back(isExact);
        xDimIndex_[name]  = xDimName_.size();
        maxDataIndex_    *= nData;
        createXData(name, nData);
        scheduleLayoutInit();
    }
}

void FitInterface::addYDim(const string name)
{
    yDimName_.push_back(name);
    yDataIndex_.push_back(map<Index, bool>());
    yDimIndex_[name] = yDimName_.size();
    createYData(name);
    scheduleLayoutInit();
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

Index FitInterface::getMaxDataIndex(void) const
{
    return maxDataIndex_;
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
    scheduleLayoutInit();
}

// variance interface //////////////////////////////////////////////////////////
void FitInterface::assumeXExact(const bool isExact, const Index i)
{
    checkXDim(i);
    xIsExact_[i] = isExact;
    scheduleLayoutInit();
}

void FitInterface::addCorr(set<array<Index, 4>> &s, const bool isCorr,
                           const array<Index, 4> &c)
{
    if (isCorr)
    {
        s.insert(c);
    }
    else
    {
        auto it = s.find(c);
        
        if (it != s.end())
        {
            s.erase(it);
        }
    }
}

void FitInterface::assumeXXCorrelated(const bool isCorr, const Index r1,
                                      const Index i1, const Index r2,
                                      const Index i2)
{
    array<Index, 4> c{{r1, i1, r2, i2}};
    
    checkXIndex(r1, i1);
    checkXIndex(r2, i2);
    if ((i1 != i2) or (r1 != r2))
    {
        addCorr(xxCorr_, isCorr, c);
    }
    scheduleFitVarMatInit();
}

void FitInterface::assumeYYCorrelated(const bool isCorr, const Index k1,
                                      const Index j1, const Index k2,
                                      const Index j2)
{
    array<Index, 4> c{{k1, j1, k2, j2}};
    
    checkPoint(k1, j1);
    checkPoint(k2, j2);
    if ((j1 != j2) or (k1 != k2))
    {
        addCorr(yyCorr_, isCorr, c);
    }
    scheduleFitVarMatInit();
}

void FitInterface::assumeXYCorrelated(const bool isCorr, const Index r,
                                      const Index i, const Index k,
                                      const Index j)
{
    array<Index, 4> c{{r, i, k, j}};
    
    checkXIndex(r, i);
    checkPoint(k, j);
    addCorr(xyCorr_, isCorr, c);
    scheduleFitVarMatInit();
}

// tests ///////////////////////////////////////////////////////////////////////
bool FitInterface::pointExists(const Index k) const
{
    bool isUsed = false;
    
    for (Index j = 0; j < getNYDim(); ++j)
    {
        isUsed = isUsed or pointExists(k, j);
    }
    
    return isUsed;
}

bool FitInterface::pointExists(const Index k, const Index j) const
{
    checkYDim(j);
    
    return !(yDataIndex_[j].find(k) == yDataIndex_[j].end());
}

bool FitInterface::isXUsed(const Index r, const Index i, const bool inFit) const
{
    vector<Index> v;
    
    checkXDim(i);
    for (Index j = 0; j < getNYDim(); ++j)
    {
        for (auto &p: yDataIndex_[j])
        {
            if (p.second or !inFit)
            {
                v      = dataCoord(p.first);
                if (v[i] == r)
                {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool FitInterface::isFitPoint(const Index k, const Index j) const
{
    checkPoint(k, j);
    
    return yDataIndex_[j].at(k);
}

// make correlation filter for fit variance matrix /////////////////////////////
DMat FitInterface::makeCorrFilter(void)
{
    updateLayout();
    
    DMat  f = DMat::Identity(layout.totalSize, layout.totalSize);
    Index row, col;
    
    for (auto &c: xxCorr_)
    {
        row = indX(c[0], c[1]);
        col = indX(c[2], c[3]);
        if ((row != -1) and (col != -1))
        {
            f(row, col) = 1.;
            f(col, row) = 1.;
        }
    }
    for (auto &c: yyCorr_)
    {
        row = indY(c[0], c[1]);
        col = indY(c[2], c[3]);
        if ((row != -1) and (col != -1))
        {
            f(row, col) = 1.;
            f(col, row) = 1.;
        }
    }
    for (auto &c: xyCorr_)
    {
        row = indX(c[0], c[1]);
        col = indY(c[2], c[3]);
        if ((row != -1) and (col != -1))
        {
            f(row, col) = 1.;
            f(col, row) = 1.;
        }
    }
    
    return f;
}

// schedule variance matrix initialization /////////////////////////////////////
void FitInterface::scheduleFitVarMatInit(const bool init)
{
    initVarMat_ = init;
}

// register a data point ///////////////////////////////////////////////////////
void FitInterface::registerDataPoint(const Index k, const Index j)
{
    checkYDim(j);
    yDataIndex_[j][k] = true;
    scheduleLayoutInit();
}

// global layout management ////////////////////////////////////////////////////
void FitInterface::scheduleLayoutInit(void)
{
    initLayout_ = true;
    scheduleFitVarMatInit();
}

bool FitInterface::initVarMat(void)
{
    return initVarMat_;
}

void FitInterface::updateLayout(void)
{
    if (initLayout_)
    {
        Index size, ifit;
        
        layout.nXFitDim   = 0;
        layout.nYFitDim   = 0;
        layout.totalSize  = 0;
        layout.totalXSize = 0;
        layout.totalYSize = 0;
        layout.xSize.clear();
        layout.ySize.clear();
        layout.xDim.clear();
        layout.yDim.clear();
        layout.xFitDim.clear();
        layout.yFitDim.clear();
        layout.x.clear();
        layout.y.clear();
        layout.xFit.clear();
        layout.yFit.clear();
        ifit = 0;
        for (Index i = 0; i < getNXDim(); ++i)
        {
            if (!xIsExact_[i])
            {
                layout.nXFitDim++;
                size = getXFitSize(i);
                layout.xSize.push_back(size);
                layout.totalXSize += size;
                layout.xDim.push_back(i);
                layout.xFitDim.push_back(layout.xDim.size() - 1);
                layout.x.push_back(vector<Index>());
                layout.xFit.push_back(vector<Index>());
                for (Index r = 0; r < getXSize(i); ++r)
                {
                    if (isXUsed(r, i))
                    {
                        layout.x[ifit].push_back(r);
                        layout.xFit[i].push_back(layout.x[ifit].size() - 1);
                    }
                    else
                    {
                        layout.xFit[i].push_back(-1);
                    }
                }
                ifit++;
            }
            else
            {
                layout.xFitDim.push_back(-1);
                layout.xFit.push_back(vector<Index>());
                for (Index r = 0; r < getXSize(i); ++r)
                {
                    layout.xFit[i].push_back(-1);
                }
            }
        }
        for (Index j = 0; j < getNYDim(); ++j)
        {
            Index s = 0;
            
            layout.nYFitDim++;
            size = getYFitSize(j);
            layout.ySize.push_back(size);
            layout.totalYSize += size;
            layout.yDim.push_back(j);
            layout.yFitDim.push_back(layout.yDim.size() - 1);
            layout.y.push_back(vector<Index>());
            layout.yFit.push_back(vector<Index>());
            layout.data.push_back(vector<Index>());
            layout.yFitFromData.push_back(map<Index, Index>());
            for (auto &p: yDataIndex_[j])
            {
                if (p.second)
                {
                    layout.y[j].push_back(s);
                    layout.yFit[j].push_back(layout.y[j].size() - 1);
                    layout.data[j].push_back(p.first);
                    layout.yFitFromData[j][p.first] = layout.y[j].size() - 1;
                }
                else
                {
                    layout.yFit[j].push_back(-1);
                    layout.yFitFromData[j][p.first] = -1;
                }
                s++;
            }
        }
        layout.totalSize = layout.totalXSize + layout.totalYSize;
        layout.nXFitDim  = static_cast<Index>(layout.xSize.size());
        layout.nYFitDim  = static_cast<Index>(layout.ySize.size());
        initLayout_      = false;
    }
}

Index FitInterface::indX(const Index r, const Index i) const
{
    Index ind = -1;
    
    if (layout.xFit[i][r] != -1)
    {
        Index ifit = layout.xFitDim[i], rfit = layout.xFit[i][r];
        
        ind = layout.totalYSize;
        for (Index a = 0; a < ifit; ++a)
        {
            ind += layout.xSize[a];
        }
        ind += rfit;
    }
    
    return ind;
}

Index FitInterface::indY(const Index k, const Index j) const
{
    Index ind = -1;
    
    if (layout.yFitFromData[j].at(k) != -1)
    {
        Index jfit = layout.yFitDim[j], sfit = layout.yFitFromData[j].at(k);
        
        ind = 0;
        for (Index b = 0; b < jfit; ++b)
        {
            ind += layout.ySize[b];
        }
        ind += sfit;
    }
    
    return ind;
}

// IO //////////////////////////////////////////////////////////////////////////
ostream & Latan::operator<<(ostream &out, FitInterface &f)
{
    out << "X dimensions: " << f.getNXDim() << endl;
    for (Index i = 0; i < f.getNXDim(); ++i)
    {
        out << "  * " << i << " \"" << f.xDimName_[i] << "\": ";
        out << f.getXSize(i) << " value(s)";
        if (f.xIsExact_[i])
        {
            out << " (assumed exact)";
        }
        out << endl;
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
    out << "X/X correlations (r1 i1 r2 i2): ";
    if (f.xxCorr_.empty())
    {
        out << "no" << endl;
    }
    else
    {
        out << endl;
        for (auto &c: f.xxCorr_)
        {
            out << "  * ";
            for (auto i: c)
            {
                out << i << " ";
            }
            out << endl;
        }
    }
    out << "Y/Y correlations (k1 j1 k2 j2): ";
    if (f.yyCorr_.empty())
    {
        out << "no" << endl;
    }
    else
    {
        out << endl;
        for (auto &c: f.yyCorr_)
        {
            out << "  * ";
            for (auto i: c)
            {
                out << i << " ";
            }
            out << endl;
        }
    }
    out << "X/Y correlations (r i k j): ";
    if (f.xyCorr_.empty())
    {
        out << "no";
    }
    else
    {
        out << endl;
        for (auto &c: f.xyCorr_)
        {
            out << "  * ";
            for (auto i: c)
            {
                out << i << " ";
            }
            out << endl;
        }
    }
    
    return out;
}
