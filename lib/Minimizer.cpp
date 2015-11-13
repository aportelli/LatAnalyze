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

// constructor /////////////////////////////////////////////////////////////////
Minimizer::Minimizer(const Index dim)
: Solver(dim)
{
    resize(dim);
}

// access //////////////////////////////////////////////////////////////////////
void Minimizer::resize(const Index dim)
{
    Solver::resize(dim);
    highLimit_.conservativeResize(dim);
    lowLimit_.conservativeResize(dim);
    hasHighLimit_.conservativeResize(dim);
    hasLowLimit_.conservativeResize(dim);
}

// limits //////////////////////////////////////////////////////////////////////
double Minimizer::getHighLimit(const Index i) const
{
    return highLimit_(i);
}

const DVec & Minimizer::getHighLimit(const PlaceHolder ph __dumb) const
{
    return highLimit_;
}

double Minimizer::getLowLimit(const Index i) const
{
    return lowLimit_(i);
}

const DVec & Minimizer::getLowLimit(const PlaceHolder ph __dumb) const
{
    return lowLimit_;
}

bool Minimizer::hasHighLimit(const Index i) const
{
    return hasHighLimit_(i);
}

bool Minimizer::hasLowLimit(const Index i) const
{
    return hasLowLimit_(i);
}

void Minimizer::setHighLimit(const Index i, const double l)
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid limit index");
    }
    else
    {
        highLimit_(i) = l;
        useHighLimit(i);
    }
}

void Minimizer::setHighLimit(const PlaceHolder ph __dumb, const DVec &l)
{
    if (l.size() != getDim())
    {
        LATAN_ERROR(Size, "invalid limit vector size");
    }
    else
    {
        highLimit_ = l;
        useHighLimit(_);
    }
}

void Minimizer::setLowLimit(const Index i, const double l)
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid limit index");
    }
    else
    {
        lowLimit_(i) = l;
        useLowLimit(i);
    }
}

void Minimizer::setLowLimit(const PlaceHolder ph __dumb, const DVec &l)
{
    if (l.size() != getDim())
    {
        LATAN_ERROR(Size, "invalid limit vector size");
    }
    else
    {
        lowLimit_ = l;
        useLowLimit(_);
    }
}

void Minimizer::useHighLimit(const Index i, const bool use)
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid limit index");
    }
    else
    {
        hasHighLimit_(i) = use;
    }
}

void Minimizer::useHighLimit(const PlaceHolder ph __dumb, const bool use)
{
    hasHighLimit_.fill(use);
}

void Minimizer::useLowLimit(const Index i, const bool use)
{
    if (i >= getDim())
    {
        LATAN_ERROR(Size, "invalid limit index");
    }
    else
    {
        hasLowLimit_(i) = use;
    }
}

void Minimizer::useLowLimit(const PlaceHolder ph __dumb, const bool use)
{
    hasLowLimit_.fill(use);
}
