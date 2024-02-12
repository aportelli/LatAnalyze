/*
 * Math.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Core/Math.hpp>
#include <LatAnalyze/Numerical/GslFFT.hpp>
#include <LatAnalyze/includes.hpp>
#include <gsl/gsl_cdf.h>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                             Custom math functions                          *
 ******************************************************************************/
DMat MATH_NAMESPACE::varToCorr(const DMat &var)
{
    DMat res = var;
    DVec invDiag = res.diagonal();
    
    invDiag = invDiag.cwiseInverse().cwiseSqrt();
    res     = (invDiag*invDiag.transpose()).cwiseProduct(res);
    
    return res;
}

DMat MATH_NAMESPACE::corrToVar(const DMat &corr, const DVec &varDiag)
{
    DMat res = corr;
    DVec varSqrtDiag = varDiag.cwiseSqrt();
    
    res = (varSqrtDiag*varSqrtDiag.transpose()).cwiseProduct(res);

    return res;
}

double MATH_NAMESPACE::conditionNumber(const DMat &mat)
{
    DVec s = mat.singularValues();

    return s.maxCoeff()/s.minCoeff();
}

double MATH_NAMESPACE::cdr(const DMat &mat)
{
    return 10.*log10(conditionNumber(mat));
}

template <typename FFT>
double nsdr(const DMat &m)
{
    Index  n = m.rows();
    FFT    fft(n);
    CMat   buf(n, 1);

    FOR_VEC(buf, i)
    {
        buf(i) = 0.;
        for (Index j = 0; j < n; ++j)
        {
            buf(i) += m(j, (i+j) % n);
        }
        buf(i) /= n;
    }
    fft(buf, FFT::Forward);

    return 10.*log10(buf.real().maxCoeff()/buf.real().minCoeff());
}

double MATH_NAMESPACE::nsdr(const DMat &mat)
{
    return ::nsdr<GslFFT>(mat);
}

/******************************************************************************
 *                             Standard C functions                           *
 ******************************************************************************/

#define DEF_STD_FUNC_1ARG(name) \
auto name##VecFunc = [](const double arg[1]){return (name)(arg[0]);};\
DoubleFunction STDMATH_NAMESPACE::name(name##VecFunc, 1);

#define DEF_STD_FUNC_2ARG(name) \
auto name##VecFunc = [](const double arg[2]){return (name)(arg[0], arg[1]);};\
DoubleFunction STDMATH_NAMESPACE::name(name##VecFunc, 2);

// Trigonometric functions
DEF_STD_FUNC_1ARG(cos)
DEF_STD_FUNC_1ARG(sin)
DEF_STD_FUNC_1ARG(tan)
DEF_STD_FUNC_1ARG(acos)
DEF_STD_FUNC_1ARG(asin)
DEF_STD_FUNC_1ARG(atan)
DEF_STD_FUNC_2ARG(atan2)

// Hyperbolic functions
DEF_STD_FUNC_1ARG(cosh)
DEF_STD_FUNC_1ARG(sinh)
DEF_STD_FUNC_1ARG(tanh)
DEF_STD_FUNC_1ARG(acosh)
DEF_STD_FUNC_1ARG(asinh)
DEF_STD_FUNC_1ARG(atanh)

// Exponential and logarithmic functions
DEF_STD_FUNC_1ARG(exp)
DEF_STD_FUNC_1ARG(log)
DEF_STD_FUNC_1ARG(log10)
DEF_STD_FUNC_1ARG(exp2)
DEF_STD_FUNC_1ARG(expm1)
DEF_STD_FUNC_1ARG(log1p)
DEF_STD_FUNC_1ARG(log2)

// Power functions
DEF_STD_FUNC_2ARG(pow)
DEF_STD_FUNC_1ARG(sqrt)
DEF_STD_FUNC_1ARG(cbrt)
DEF_STD_FUNC_2ARG(hypot)

// Error and gamma functions
DEF_STD_FUNC_1ARG(erf)
DEF_STD_FUNC_1ARG(erfc)
DEF_STD_FUNC_1ARG(tgamma)
DEF_STD_FUNC_1ARG(lgamma)

// Rounding and remainder functions
DEF_STD_FUNC_1ARG(ceil)
DEF_STD_FUNC_1ARG(floor)
DEF_STD_FUNC_2ARG(fmod)
DEF_STD_FUNC_1ARG(trunc)
DEF_STD_FUNC_1ARG(round)
DEF_STD_FUNC_1ARG(rint)
DEF_STD_FUNC_1ARG(nearbyint)
DEF_STD_FUNC_2ARG(remainder)

// Minimum, maximum, difference functions
DEF_STD_FUNC_2ARG(fdim)
DEF_STD_FUNC_2ARG(fmax)
DEF_STD_FUNC_2ARG(fmin)

// Absolute value
DEF_STD_FUNC_1ARG(fabs)

// p-value
auto chi2PValueVecFunc = [](const double arg[2])
{
    return 2.*min(gsl_cdf_chisq_P(arg[0], arg[1]), gsl_cdf_chisq_Q(arg[0], arg[1]));
};

auto chi2CcdfVecFunc = [](const double arg[2])
{
    return gsl_cdf_chisq_Q(arg[0], arg[1]);
};

DoubleFunction MATH_NAMESPACE::chi2PValue(chi2PValueVecFunc, 2);
DoubleFunction MATH_NAMESPACE::chi2Ccdf(chi2CcdfVecFunc, 2);
