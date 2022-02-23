/*
 * Math.hpp, part of LatAnalyze 3
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

#ifndef Latan_Math_hpp_
#define	Latan_Math_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Functional/Function.hpp>
#include <LatAnalyze/Core/MathInterpreter.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                             Custom math functions                          *
 ******************************************************************************/
#define MATH_NAMESPACE Math

namespace MATH_NAMESPACE
{
    // integer power function
    template <unsigned int n, typename T>
    typename std::enable_if<(n == 0), T>::type pow(const T x __dumb)
    {
        return 1;
    }
    
    template <unsigned int n, typename T>
    typename std::enable_if<(n == 1), T>::type pow(const T x)
    {
        return x;
    }
    
    template <unsigned int n, typename T>
    typename std::enable_if<(n > 1), T>::type pow(const T x)
    {
        return x*pow<n-1>(x);
    }
    
    // integral factorial function
    template <typename T>
    T factorial(const T n)
    {
        static_assert(std::is_integral<T>::value,
                      "factorial must me used with an integral argument");
        
        T res = n;
        for (T i = n - 1; i != 0; --i)
        {
            res *= i;
        }
        
        return res;
    }
    
    // convert variance matrix to correlation matrix
    DMat varToCorr(const DMat &var);
    DMat corrToVar(const DMat &corr, const DVec &varDiag);

    // matrix SVD dynamic range
    double svdDynamicRange(const DMat &mat);
    double svdDynamicRangeDb(const DMat &mat);
    
    // Constants
    constexpr double pi  = 3.1415926535897932384626433832795028841970;
    constexpr double e   = 2.7182818284590452353602874713526624977572;
    constexpr double inf = std::numeric_limits<double>::infinity();
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
}


/******************************************************************************
 *                             Standard C functions                           *
 ******************************************************************************/
#define STDMATH_NAMESPACE StdMath

#define DECL_STD_FUNC(name) \
namespace STDMATH_NAMESPACE\
{\
    extern DoubleFunction name;\
}

// Trigonometric functions
DECL_STD_FUNC(cos)
DECL_STD_FUNC(sin)
DECL_STD_FUNC(tan)
DECL_STD_FUNC(acos)
DECL_STD_FUNC(asin)
DECL_STD_FUNC(atan)
DECL_STD_FUNC(atan2)

// Hyperbolic functions
DECL_STD_FUNC(cosh)
DECL_STD_FUNC(sinh)
DECL_STD_FUNC(tanh)
DECL_STD_FUNC(acosh)
DECL_STD_FUNC(asinh)
DECL_STD_FUNC(atanh)

// Exponential and logarithmic functions
DECL_STD_FUNC(exp)
DECL_STD_FUNC(log)
DECL_STD_FUNC(log10)
DECL_STD_FUNC(exp2)
DECL_STD_FUNC(expm1)
DECL_STD_FUNC(log1p)
DECL_STD_FUNC(log2)

// Power functions
DECL_STD_FUNC(pow)
DECL_STD_FUNC(sqrt)
DECL_STD_FUNC(cbrt)
DECL_STD_FUNC(hypot)

// Error and gamma functions
DECL_STD_FUNC(erf)
DECL_STD_FUNC(erfc)
DECL_STD_FUNC(tgamma)
DECL_STD_FUNC(lgamma)

// Rounding and remainder functions
DECL_STD_FUNC(ceil)
DECL_STD_FUNC(floor)
DECL_STD_FUNC(fmod)
DECL_STD_FUNC(trunc)
DECL_STD_FUNC(round)
DECL_STD_FUNC(rint)
DECL_STD_FUNC(nearbyint)
DECL_STD_FUNC(remainder)

// Minimum, maximum, difference functions
DECL_STD_FUNC(fdim)
DECL_STD_FUNC(fmax)
DECL_STD_FUNC(fmin)

// Absolute value
DECL_STD_FUNC(fabs)

/******************************************************************************
 *                                Other functions                             *
 ******************************************************************************/
// p-value
namespace MATH_NAMESPACE
{
    extern DoubleFunction chi2PValue;
    extern DoubleFunction chi2Ccdf;
}

END_LATAN_NAMESPACE

#endif // Latan_Math_hpp_
