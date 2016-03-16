/*
 * XYStatData.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/XYStatData.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Math.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                          FitResult implementation                          *
 ******************************************************************************/
// access //////////////////////////////////////////////////////////////////////
double FitResult::getChi2(void) const
{
    return chi2_;
}

double FitResult::getChi2PerDof(void) const
{
    return chi2_/getNDof();
}

double FitResult::getNDof(void) const
{
    return static_cast<double>(nDof_);
}

double FitResult::getPValue(void) const
{
    return Math::chi2PValue(getChi2(), getNDof());;
}

const DoubleFunction & FitResult::getModel(const Index j) const
{
    return model_[static_cast<unsigned int>(j)];
}

/******************************************************************************
 *                       XYStatData implementation                            *
 ******************************************************************************/
// data access /////////////////////////////////////////////////////////////////
double & XYStatData::x(const Index vi, const Index i)
{
    checkXIndex(vi, i);
    scheduleXMapInit();
    scheduleChi2DataVecInit();
    
    return xData_[i](vi);
}

const double & XYStatData::x(const Index vi, const Index i) const
{
    checkXIndex(vi, i);
    
    return xData_[i](vi);
}

double & XYStatData::y(const Index k, const Index j)
{
    checkYDim(j);
    if (!pointExists(k, j))
    {
        registerDataPoint(k, j);
        resizeVarMat();
    }
    scheduleXMapInit();
    scheduleChi2DataVecInit();
    
    return yData_[j][k];
}

const double & XYStatData::y(const Index k, const Index j) const
{
    checkPoint(k, j);
    
    return yData_[j].at(k);
}

void XYStatData::setXXVar(const Index i1, const Index i2, const DMat &m)
{
    checkXDim(i1);
    checkXDim(i2);
    checkVarMat(m, xxVar_(i1, i2));
    xxVar_(i1, i2) = m;
    if (i1 != i2)
    {
        xxVar_(i2, i1) = m.transpose();
    }
    scheduleFitVarMatInit();
}

void XYStatData::setYYVar(const Index j1, const Index j2, const DMat &m)
{
    checkYDim(j1);
    checkYDim(j2);
    checkVarMat(m, yyVar_(j1, j2));
    yyVar_(j1, j2) = m;
    if (j1 != j2)
    {
        yyVar_(j2, j1) = m.transpose();
    }
    scheduleFitVarMatInit();
}

void XYStatData::setXYVar(const Index i, const Index j, const DMat &m)
{
    checkXDim(i);
    checkYDim(j);
    checkVarMat(m, xyVar_(i, j));
    xyVar_(i, j) = m;
    scheduleFitVarMatInit();
}

void XYStatData::setXError(const Index i, const DVec &err)
{
    checkXDim(i);
    checkErrVec(err, xxVar_(i, i));
    xxVar_(i, i).diagonal() = err.cwiseProduct(err);
    scheduleFitVarMatInit();
}

void XYStatData::setYError(const Index j, const DVec &err)
{
    checkXDim(j);
    checkErrVec(err, yyVar_(j, j));
    yyVar_(j, j).diagonal() = err.cwiseProduct(err);
    scheduleFitVarMatInit();
}

const DMat & XYStatData::getXXVar(const Index i1, const Index i2) const
{
    checkXDim(i1);
    checkXDim(i2);
    
    return xxVar_(i1, i2);
}

const DMat & XYStatData::getYYVar(const Index j1, const Index j2) const
{
    checkYDim(j1);
    checkYDim(j2);
    
    return yyVar_(j1, j2);
}

const DMat & XYStatData::getXYVar(const Index i, const Index j) const
{
    checkXDim(i);
    checkYDim(j);
    
    return xyVar_(i, j);
}

DVec XYStatData::getXError(const Index i) const
{
    checkXDim(i);
    
    return xxVar_(i, i).diagonal().cwiseSqrt();
}

DVec XYStatData::getYError(const Index j) const
{
    checkXDim(j);
    
    return yyVar_(j, j).diagonal().cwiseSqrt();
}

// get total fit variance matrix ///////////////////////////////////////////////
const DMat & XYStatData::getFitVarMat(void)
{
    updateFitVarMat();
    
    return fitVar_;
}

const DMat & XYStatData::getFitVarMatPInv(void)
{
    updateFitVarMat();
    
    return fitVarInv_;
}

// fit /////////////////////////////////////////////////////////////////////////
FitResult XYStatData::fit(Minimizer &minimizer, const DVec &init,
                          const vector<const DoubleModel *> &v)
{
    // check model consistency
    checkModelVec(v);
    
    // buffering
    updateLayout();
    updateFitVarMat();
    updateChi2DataVec();
    
    // get number of parameters
    Index nPar      = v[0]->getNPar();
    Index totalNPar = nPar + layout.totalXSize;
    
    // chi^2 function
    auto chi2Func = [this, totalNPar, &v](const double *x)->double
    {
        ConstMap<DVec> p(x, totalNPar);
        
        updateChi2ModVec(p, v);
        chi2Vec_ = (chi2ModVec_ - chi2DataVec_);
        
        return (chi2Vec_.transpose()*fitVarInv_).dot(chi2Vec_);
    };
    DoubleFunction chi2(chi2Func, totalNPar);
    
    // minimization
    FitResult result;
    DVec      totalInit(totalNPar);
    
    totalInit.segment(0, nPar) = init;
    totalInit.segment(nPar, layout.totalXSize) =
        chi2DataVec_.segment(layout.totalYSize, layout.totalXSize);
    minimizer.setInit(totalInit);
    result       = minimizer(chi2);
    result.chi2_ = chi2(result);
    result.nDof_ = layout.totalYSize - nPar;
    result.model_.resize(v.size());
    for (unsigned int j = 0; j < v.size(); ++j)
    {
        result.model_[j] = v[j]->fixPar(result);
    }
    
    return result;
}

// create data /////////////////////////////////////////////////////////////////
void XYStatData::createXData(const Index nData)
{
    xData_.push_back(DVec::Zero(nData));
    resizeVarMat();
}

void XYStatData::createYData(void)
{
    yData_.push_back(map<Index, double>());
    resizeVarMat();
}

void XYStatData::resizeVarMat(void)
{
    xxVar_.conservativeResize(getNXDim(), getNXDim());
    for (Index i1 = 0; i1 < getNXDim(); ++i1)
    for (Index i2 = 0; i2 < getNXDim(); ++i2)
    {
        xxVar_(i1, i2).conservativeResize(getXSize(i1), getXSize(i2));
    }
    yyVar_.conservativeResize(getNYDim(), getNYDim());
    for (Index j1 = 0; j1 < getNYDim(); ++j1)
    for (Index j2 = 0; j2 < getNYDim(); ++j2)
    {
        yyVar_(j1, j2).conservativeResize(getYSize(j1), getYSize(j2));
    }
    xyVar_.conservativeResize(getNXDim(), getNYDim());
    for (Index i = 0; i < getNXDim(); ++i)
    for (Index j = 0; j < getNYDim(); ++j)
    {
        xyVar_(i, j).conservativeResize(getXSize(i), getYSize(j));
    }
    scheduleFitVarMatInit();
}

// schedule buffer computation /////////////////////////////////////////////////
void XYStatData::scheduleXMapInit(void)
{
    initXMap_ = true;
}

void XYStatData::scheduleChi2DataVecInit(void)
{
    initChi2DataVec_ = true;
}

// buffer total fit variance matrix ////////////////////////////////////////////
void XYStatData::updateFitVarMat(void)
{
    if (initVarMat())
    {
        updateLayout();
        
        DMat  &v = fitVar_;
        Index roffs, coffs;
        
        v.resize(layout.totalSize, layout.totalSize);
        roffs = layout.totalYSize;
        for (Index ifit1 = 0; ifit1 < layout.nXFitDim; ++ifit1)
        {
            coffs = layout.totalYSize;
            for (Index ifit2 = 0; ifit2 < layout.nXFitDim; ++ifit2)
            {
                for (Index rfit1 = 0; rfit1 < layout.xSize[ifit1]; ++rfit1)
                for (Index rfit2 = 0; rfit2 < layout.xSize[ifit2]; ++rfit2)
                {
                    Index i1, i2, r1, r2;
                    
                    i1 = layout.xDim[ifit1];
                    i2 = layout.xDim[ifit2];
                    r1 = layout.x[ifit1][rfit1];
                    r2 = layout.x[ifit2][rfit2];
                    
                    v(roffs+rfit1, coffs+rfit2) = xxVar_(i1, i2)(r1, r2);
                    v(coffs+rfit2, roffs+rfit1) = v(roffs+rfit1, coffs+rfit2);
                }
                coffs += layout.xSize[ifit2];
            }
            roffs += layout.xSize[ifit1];
        }
        roffs = 0;
        for (Index jfit1 = 0; jfit1 < layout.nYFitDim; ++jfit1)
        {
            coffs = 0;
            for (Index jfit2 = 0; jfit2 < layout.nYFitDim; ++jfit2)
            {
                for (Index sfit1 = 0; sfit1 < layout.ySize[jfit1]; ++sfit1)
                for (Index sfit2 = 0; sfit2 < layout.ySize[jfit2]; ++sfit2)
                {
                    Index j1, j2, s1, s2;
                    
                    j1 = layout.yDim[jfit1];
                    j2 = layout.yDim[jfit2];
                    s1 = layout.y[jfit1][sfit1];
                    s2 = layout.y[jfit2][sfit2];
                    
                    v(roffs+sfit1, coffs+sfit2) = yyVar_(j1, j2)(s1, s2);
                    v(coffs+sfit2, roffs+sfit1) = v(roffs+sfit1, coffs+sfit2);
                }
                coffs += layout.ySize[jfit2];
            }
            roffs += layout.ySize[jfit1];
        }
        roffs = layout.totalYSize;
        for (Index ifit = 0; ifit < layout.nXFitDim; ++ifit)
        {
            coffs = 0;
            for (Index jfit = 0; jfit < layout.nYFitDim; ++jfit)
            {
                for (Index rfit = 0; rfit < layout.xSize[ifit]; ++rfit)
                for (Index sfit = 0; sfit < layout.ySize[jfit]; ++sfit)
                {
                    Index i, j, r, s;
                    
                    i = layout.xDim[ifit];
                    j = layout.yDim[jfit];
                    r = layout.x[ifit][rfit];
                    s = layout.y[jfit][sfit];
                    
                    v(roffs+rfit, coffs+sfit) = xyVar_(i, j)(r, s);
                    v(coffs+sfit, roffs+rfit) = v(roffs+rfit, coffs+sfit);
                }
                coffs += layout.ySize[jfit];
            }
            roffs += layout.xSize[ifit];
        }
        chi2DataVec_.resize(layout.totalSize);
        chi2ModVec_.resize(layout.totalSize);
        chi2Vec_.resize(layout.totalSize);
        fitVar_    = fitVar_.cwiseProduct(makeCorrFilter());
        fitVarInv_ = fitVar_.pInverse();
        scheduleFitVarMatInit(false);
    }
}

// buffer list of x vectors ////////////////////////////////////////////////////
void XYStatData::updateXMap(void)
{
    if (initXMap_)
    {
        vector<Index> v;
        set<Index>    indSet;
        
        for (Index j = 0; j < getNYDim(); ++j)
        {
            for (auto &p: yData_[j])
            {
                indSet.insert(p.first);
            }
        }
        xMap_.clear();
        for (auto k: indSet)
        {
            xMap_[k] = DVec(getNXDim());
            v = dataCoord(k);
            for (Index i = 0; i < getNXDim(); ++i)
            {
                xMap_[k](i) = xData_[i](v[i]);
            }
        }
        initXMap_ = false;
    }
}

// buffer chi^2 vectors ////////////////////////////////////////////////////////
void XYStatData::updateChi2DataVec(void)
{
    if (initChi2DataVec_)
    {
        Index a = 0, j, k, i, r;
        
        updateLayout();
        for (Index jfit = 0; jfit < layout.nYFitDim; ++jfit)
        for (Index sfit = 0; sfit < layout.ySize[jfit]; ++sfit)
        {
            j               = layout.yDim[jfit];
            k               = layout.data[jfit][sfit];
            chi2DataVec_(a) = yData_[j][k];
            a++;
        }
        for (Index ifit = 0; ifit < layout.nXFitDim; ++ifit)
        for (Index rfit = 0; rfit < layout.xSize[ifit]; ++rfit)
        {
            i               = layout.xDim[ifit];
            r               = layout.x[ifit][rfit];
            chi2DataVec_(a) = xData_[i](r);
            a++;
        }
        initChi2DataVec_ = false;
    }
}

void XYStatData::updateChi2ModVec(const DVec p,
                                  const vector<const DoubleModel *> &v)
{
    updateLayout();
    
    Index nPar = v[0]->getNPar(), a = 0, j, k;
    auto  &par = p.segment(0, nPar), &xsi = p.segment(nPar, layout.totalXSize);
    
    updateXMap();
    for (Index jfit = 0; jfit < layout.nYFitDim; ++jfit)
    for (Index sfit = 0; sfit < layout.ySize[jfit]; ++sfit)
    {
        j              = layout.yDim[jfit];
        k              = layout.data[jfit][sfit];
        chi2ModVec_(a) = (*v[j])(xMap_[k].data(), par.data());
        a++;
    }
    chi2ModVec_.segment(a, layout.totalXSize) = xsi;
}


