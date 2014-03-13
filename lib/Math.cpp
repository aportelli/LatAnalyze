/*
 * Math.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                             Standard C functions                           *
 ******************************************************************************/

#define DEF_STD_FUNC_1ARG(name) \
auto name##VecFunc = [](const double arg[1]){return (name)(arg[0]);};\
DoubleFunction STDMATH_NAMESPACE::name(1, name##VecFunc);

#define DEF_STD_FUNC_2ARG(name) \
auto name##VecFunc = [](const double arg[2]){return (name)(arg[0], arg[1]);};\
DoubleFunction STDMATH_NAMESPACE::name(2, name##VecFunc);

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

#define ADD_FUNC(func) fTable[#func] = &STDMATH_NAMESPACE::func
void STDMATH_NAMESPACE::addStdMathFunc(FunctionTable &fTable)
{
    // Trigonometric functions
    ADD_FUNC(cos);
    ADD_FUNC(sin);
    ADD_FUNC(tan);
    ADD_FUNC(acos);
    ADD_FUNC(asin);
    ADD_FUNC(atan);
    ADD_FUNC(atan2);
    
    // Hyperbolic functions
    ADD_FUNC(cosh);
    ADD_FUNC(sinh);
    ADD_FUNC(tanh);
    ADD_FUNC(acosh);
    ADD_FUNC(asinh);
    ADD_FUNC(atanh);
    
    // Exponential and logarithmic functions
    ADD_FUNC(exp);
    ADD_FUNC(log);
    ADD_FUNC(log10);
    ADD_FUNC(exp2);
    ADD_FUNC(expm1);
    ADD_FUNC(log1p);
    ADD_FUNC(log2);
    
    // Power functions
    ADD_FUNC(pow);
    ADD_FUNC(sqrt);
    ADD_FUNC(cbrt);
    ADD_FUNC(hypot);
    
    // Error and gamma functions
    ADD_FUNC(erf);
    ADD_FUNC(erfc);
    ADD_FUNC(tgamma);
    ADD_FUNC(lgamma);
    
    // Rounding and remainder functions
    ADD_FUNC(ceil);
    ADD_FUNC(floor);
    ADD_FUNC(fmod);
    ADD_FUNC(trunc);
    ADD_FUNC(round);
    ADD_FUNC(rint);
    ADD_FUNC(nearbyint);
    ADD_FUNC(remainder);
    
    // Minimum, maximum, difference functions
    ADD_FUNC(fdim);
    ADD_FUNC(fmax);
    ADD_FUNC(fmin);
    
    // Absolute value
    ADD_FUNC(fabs);
}
