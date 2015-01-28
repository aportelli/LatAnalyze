/*
 * Derivative.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Derivative.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Math.hpp>
#include <gsl/gsl_deriv.h>

using namespace std;
using namespace Latan;
using namespace Math;

/******************************************************************************
 *                       Derivative implementation                            *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
Derivative::Derivative(const DoubleFunction &f, const Index dir,
                       const double step)
: DoubleFunction(f.getNArg())
, f_(&f)
, dir_(dir)
, step_(step)
, buffer_(new DVec(f.getNArg()))
{}

Derivative::Derivative(const DoubleFunction &f, const Index dir,
                       const Index order, const DVec point, const double step)
: Derivative(f, dir, step)
{
    setOrderAndPoint(order, point);
}

// access //////////////////////////////////////////////////////////////////////
Index Derivative::getOrder(void) const
{
    return order_;
}

Index Derivative::getNPoint(void) const
{
    return point_.size();
}

double Derivative::getStep(void) const
{
    return step_;
}

void Derivative::setFunction(const DoubleFunction &f)
{
    f_ = &f;
}

void Derivative::setOrderAndPoint(const Index order, const DVec point)
{
    if (order >= point.size())
    {
        LATAN_ERROR(Size, "derivative order is superior or equal to the number of point");
    }
    order_ = order;
    point_ = point;
    coefficient_.resize(point.size());
    makeCoefficients();
}

void Derivative::setStep(const double step)
{
    step_ = step;
}
// coefficient generation //////////////////////////////////////////////////////
// from B. Fornberg, “Generation of finite difference formulas on arbitrarily
// spaced grids,” Math. Comp., vol. 51, no. 184, pp. 699–706, 1988.
// http://dx.doi.org/10.1090/S0025-5718-1988-0935077-0
void Derivative::makeCoefficients(void)
{
    double      c[3];
    const Index N = point_.size() - 1, M = order_;
    DMat        curr(M + 1, N + 1), prev(M + 1, N + 1);
    
    curr.fill(0.);
    prev.fill(0.);
    prev(0, 0) = 1.;
    c[0]       = 1.;
    for (Index n = 1; n <= N; ++n)
    {
        c[1] = 1.;
        for (Index nu = 0; nu <= n - 1; ++nu)
        {
            c[2]  = point_(n) - point_(nu);
            c[1] *= c[2];
            for (Index m = 0; m <= min(n, M); ++m)
            {
                curr(m, nu) = point_(n)*prev(m, nu);
                if (m)
                {
                    curr(m, nu) -= m*prev(m-1, nu);
                }
                curr(m, nu) /= c[2];
            }
        }
        for (Index m = 0; m <= min(n, M); ++m)
        {
            curr(m, n) = -point_(n-1)*prev(m, n-1);
            if (m)
            {
                curr(m, n) += m*prev(m-1, n-1);
            }
            curr(m, n) *= c[0]/c[1];
        }
        c[0] = c[1];
        prev = curr;
    }
    coefficient_ = curr.row(M);
}

// function call ///////////////////////////////////////////////////////////////
double Derivative::operator()(const double *x) const
{
    ConstMap<DVec> xMap(x, (*f_).getNArg());
    double         res = 0.;
    
    *buffer_ = xMap;
    FOR_VEC(point_, i)
    {
        (*buffer_)(dir_) = x[dir_] + point_(i)*step_;
        res += coefficient_[i]*(*f_)(*buffer_);
    }
    res /= pow(step_, order_);
    
    return res;
}

/******************************************************************************
 *                     CentralDerivative implementation                       *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
CentralDerivative::CentralDerivative(const DoubleFunction &f, const Index dir,
                                     const Index order, const Index precOrder)
: Derivative(f, dir)
{
    setOrder(order, precOrder);
}

// access //////////////////////////////////////////////////////////////////////
Index CentralDerivative::getPrecOrder(void) const
{
    return precOrder_;
}

void CentralDerivative::setOrder(const Index order, const Index precOrder)
{
    const Index nPoint = 2*(precOrder + (order - 1)/2) + 1;
    DVec        point(nPoint);
    
    precOrder_ = precOrder;
    FOR_VEC(point, i)
    {
        point(i) = static_cast<double>(i - (nPoint - 1)/2);
    }
    setOrderAndPoint(order, point);
    tuneStep();
}

// step tuning /////////////////////////////////////////////////////////////////
// the rounding error should be O(N*epsilon/h^order)
// 
void CentralDerivative::tuneStep(void)
{
    const Index  nPoint  = getNPoint();
    const double epsilon = numeric_limits<double>::epsilon();
    const double step    = pow(epsilon*nPoint, 1./(2.*precOrder_+getOrder()));
    
    setStep(step);
}
