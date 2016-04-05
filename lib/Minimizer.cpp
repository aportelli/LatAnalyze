/*
 * Minimizer.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Minimizer.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

// access //////////////////////////////////////////////////////////////////////
void Minimizer::resize(const Index dim)
{
    const Index oldDim = getDim();
    
    Solver::resize(dim);
    highLimit_.conservativeResize(dim);
    lowLimit_.conservativeResize(dim);
    hasHighLimit_.conservativeResize(dim);
    hasLowLimit_.conservativeResize(dim);
    if (dim > oldDim)
    {
        highLimit_.segment(oldDim, dim - oldDim).fill(0.);
        highLimit_.segment(oldDim, dim - oldDim).fill(0.);
        lowLimit_.segment(oldDim, dim - oldDim).fill(0.);
        hasHighLimit_.segment(oldDim, dim - oldDim).fill(false);
        hasLowLimit_.segment(oldDim, dim - oldDim).fill(false);
    }
}

double Minimizer::getHighLimit(const Index i) const
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid variable index");
    }
    
    return highLimit_(i);
}

const DVec & Minimizer::getHighLimit(const PlaceHolder ph __dumb) const
{
    return highLimit_;
}

double Minimizer::getLowLimit(const Index i) const
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid variable index");
    }
    
    return lowLimit_(i);
}

const DVec & Minimizer::getLowLimit(const PlaceHolder ph __dumb) const
{
    return lowLimit_;
}

bool Minimizer::hasHighLimit(const Index i) const
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid variable index");
    }
    
    return hasHighLimit_(i);
}

bool Minimizer::hasLowLimit(const Index i) const
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid variable index");
    }
    
    return hasLowLimit_(i);
}

void Minimizer::setHighLimit(const Index i, const double l)
{
    if (i >= getDim())
    {
        resize(i + 1);
    }
    highLimit_(i) = l;
    useHighLimit(i);
}

void Minimizer::setHighLimit(const PlaceHolder ph __dumb, const DVec &l)
{
    if (l.size() != getDim())
    {
        resize(l.size());
    }
    highLimit_ = l;
    useHighLimit(_);
}

void Minimizer::setLowLimit(const Index i, const double l)
{
    if (i >= getDim())
    {
        resize(i + 1);
    }
    lowLimit_(i) = l;
    useLowLimit(i);
}

void Minimizer::setLowLimit(const PlaceHolder ph __dumb, const DVec &l)
{
    if (l.size() != getDim())
    {
        resize(l.size());
    }
    lowLimit_ = l;
    useLowLimit(_);
}

void Minimizer::useHighLimit(const Index i, const bool use)
{
    if (i >= getDim())
    {
        resize(i + 1);
    }
    hasHighLimit_(i) = use;
}

void Minimizer::useHighLimit(const PlaceHolder ph __dumb, const bool use)
{
    hasHighLimit_.fill(use);
}

void Minimizer::useLowLimit(const Index i, const bool use)
{
    if (i >= getDim())
    {
        resize(i + 1);
    }
    hasLowLimit_(i) = use;
}

void Minimizer::useLowLimit(const PlaceHolder ph __dumb, const bool use)
{
    hasLowLimit_.fill(use);
}

unsigned int Minimizer::getMaxPass(void) const
{
    return maxPass_;
}

void Minimizer::setMaxPass(const unsigned int maxPass)
{
    maxPass_ = maxPass;
}
