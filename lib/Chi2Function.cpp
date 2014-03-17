/*
 * Chi2Function.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2014 Antonin Portelli
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

#include <LatAnalyze/Chi2Function.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/XYStatData.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                         Chi2Function implementation                        *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
Chi2Function::Chi2Function(const XYStatData &data)
: DoubleFunction(0)
, data_(data)
, buffer_(new Chi2FunctionBuffer)
{
    resizeBuffer();
}

Chi2Function::Chi2Function(const XYStatData &data,
                           const vector<const DoubleModel *> &modelVector)
: Chi2Function(data)
{
    setModel(modelVector);
}

// access //////////////////////////////////////////////////////////////////////
Index Chi2Function::getNArg(void) const
{
    if (nPar_ < 0)
    {
        LATAN_ERROR(Memory, "no model set");
    }
    
    return nPar_ + data_.getStatXDim()*data_.getNFitPoint();
}

Index Chi2Function::getNDof(void) const
{
    if (nPar_ < 0)
    {
        LATAN_ERROR(Memory, "no model set");
    }
    
    return data_.getYDim()*data_.getNFitPoint() - nPar_;
};

Index Chi2Function::getNPar(void) const
{
    if (nPar_ < 0)
    {
        LATAN_ERROR(Memory, "no model set");
    }
    
    return nPar_;
};

void Chi2Function::setModel(const DoubleModel &model, const Index j)
{
    if (static_cast<Index>(model_.size()) != data_.getYDim())
    {
         model_.resize(static_cast<unsigned int>(data_.getYDim()));
    }
    if (model.getNArg() != data_.getXDim())
    {
        LATAN_ERROR(Size, "model number of arguments and x-dimension mismatch");
    }
    for (unsigned int l = 0; l < data_.getYDim(); ++l)
    {
        if (model_[l]&&(l != j))
        {
            if (model_[l]->getNPar() != model.getNPar())
            {
                LATAN_ERROR(Size, "model number of parameter mismatch");
            }
        }
    }
    model_[static_cast<unsigned int>(j)] = &model;
    nPar_                                = model.getNPar();
}

void Chi2Function::setModel(const vector<const DoubleModel *> &modelVector)
{
    if (static_cast<Index>(model_.size()) != data_.getYDim())
    {
        model_.resize(static_cast<unsigned int>(data_.getYDim()));
    }
    if (modelVector.size() != static_cast<unsigned int>(data_.getYDim()))
    {
        LATAN_ERROR(Size, "number of models and y-dimension mismatch");
    }
    for (unsigned int j = 0; j < model_.size(); ++j)
    {
        if (!modelVector[j])
        {
            LATAN_ERROR(Memory, "trying to set a null model");
        }
        if (modelVector[j]->getNArg() != data_.getXDim())
        {
            LATAN_ERROR(Size, "model number of arguments and x-dimension mismatch");
        }
        model_[static_cast<unsigned int>(j)] = modelVector[j];
        if (modelVector[j]->getNPar() != modelVector[0]->getNPar())
        {
            LATAN_ERROR(Size, "model number of parameter mismatch");
        }
    }
    nPar_ = modelVector[0]->getNPar();
}

void Chi2Function::requestInit(void) const
{
    buffer_->isInit = false;
}

void Chi2Function::resizeBuffer(void) const
{
    Index size;
    
    size = (data_.getYDim() + data_.getStatXDim())*data_.getNFitPoint();
    buffer_->v.setConstant(size, 0.0);
    buffer_->x.setConstant(data_.getXDim(), 0.0);
    buffer_->invVar.setConstant(size, size, 0.0);
    buffer_->xInd.setConstant(data_.getStatXDim(), 0);
    buffer_->dInd.setConstant(data_.getNFitPoint(), 0);
}

// compute variance matrix inverse /////////////////////////////////////////////
void Chi2Function::setVarianceBlock(const Index l1, const Index l2,
                                    ConstBlock<DMatBase> m) const
{
    const Index nPoint = data_.getNFitPoint();
    
    FOR_VEC(buffer_->dInd, k2)
    FOR_VEC(buffer_->dInd, k1)
    {
        if (data_.isDataCorrelated(buffer_->dInd(k1), buffer_->dInd(k2)))
        {
            buffer_->invVar(l1*nPoint + k1, l2*nPoint + k2) =
                m(buffer_->dInd(k1), buffer_->dInd(k2));
        }
    }
}

void Chi2Function::initBuffer(void) const
{
    const Index xDim     = data_.getXDim();
    const Index statXDim = data_.getStatXDim();
    const Index yDim     = data_.getYDim();
    const Index nData    = data_.getNData();
    const Index nPoint   = data_.getNFitPoint();
    Index       is, kf;
    
    // resize buffer
    resizeBuffer();
    
    // build index tables
    is = 0;
    for (Index i = 0; i < xDim; ++i)
    {
        if (!data_.isXExact(i))
        {
            buffer_->xInd(is) = i;
            is++;
        }
    }
    kf = 0;
    for (Index k = 0; k < nData; ++k)
    {
        if (data_.isFitPoint(k))
        {
            buffer_->dInd(kf) = k;
            kf++;
        }
    }
    
    // set y/y variance matrix
    for (Index j2 = 0; j2 < yDim; ++j2)
    for (Index j1 = 0; j1 < yDim; ++j1)
    {
        if (data_.isYYCorrelated(j1, j2))
        {
            setVarianceBlock(j1, j2, data_.yyVar(j1, j2));
        }
    }

    // set x/x variance matrix
    FOR_VEC(buffer_->xInd, i2)
    FOR_VEC(buffer_->xInd, i1)
    {
        if (data_.isXXCorrelated(buffer_->xInd(i1), buffer_->xInd(i2)))
        {
            setVarianceBlock(i1 + yDim, i2 + yDim,
                             data_.xxVar(buffer_->xInd(i1), buffer_->xInd(i2)));
        }
    }

    // set y/x variance matrix
    FOR_VEC(buffer_->xInd, i)
    for (Index j = 0; j < yDim; ++j)
    {
        if (data_.isYXCorrelated(j, buffer_->xInd(i)))
        {
            setVarianceBlock(j, i + yDim, data_.yxVar(j, buffer_->xInd(i)));
        }
    }
    auto lowerYX = buffer_->invVar.block(yDim*nPoint, 0, yDim*statXDim,
                                         yDim*nPoint);
    auto upperYX = buffer_->invVar.block(0, yDim*nPoint, yDim*nPoint,
                                         yDim*statXDim);
    lowerYX = upperYX.transpose();

    // inversion
    buffer_->invVar = buffer_->invVar.inverse().eval();
    buffer_->isInit = true;
}

// function call ///////////////////////////////////////////////////////////////
double Chi2Function::operator()(const double *arg) const
{
    if (!model_[0])
    {
        LATAN_ERROR(Memory, "null model");
    }
    
    const Index    xDim   = data_.getXDim();
    const Index    yDim   = data_.getYDim();
    const Index    nPoint = data_.getNFitPoint();
    Index          is;
    ConstMap<DVec> xi(arg + nPar_, getNArg() - nPar_, 1);
    double         res;
    
    // initialize buffers if necessary
    if (!buffer_->isInit)
    {
        initBuffer();
    }
    
    // set the upper part of v: f_j(xi, p) - y_j
    is = 0;
    for (Index j = 0; j < yDim; ++j)
    FOR_VEC(buffer_->dInd, k)
    {
        const DoubleModel *f = model_[static_cast<unsigned int>(j)];
        double            f_jk, y_jk = data_.y(j, buffer_->dInd(k));
        
        if (!f)
        {
            LATAN_ERROR(Memory, "null model");
        }
        for (Index i = 0; i < xDim; ++i)
        {
            if (data_.isXExact(i))
            {
                buffer_->x(i) = data_.x(i, buffer_->dInd(k));
            }
            else
            {
                buffer_->x(i) = xi(is*nPoint + k);
                is++;
            }
        }
        // call model on double pointers to avoid any copy
        f_jk = (*f)(buffer_->x.data(), arg);
        buffer_->v(j*nPoint + k) = f_jk - y_jk;
    }
    
    // set the down part of v: xi_ik - x_ik
    FOR_VEC(buffer_->xInd, i)
    FOR_VEC(buffer_->dInd, k)
    {
        double x_ik  = data_.x(buffer_->xInd(i), buffer_->dInd(k));
        double xi_ik = xi(i*nPoint + k);
        
        buffer_->v(i*nPoint + k) = xi_ik - x_ik;
    }
    
    // compute result
    res = buffer_->v.dot(buffer_->invVar*buffer_->v);
    
    return res;
}
