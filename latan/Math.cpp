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

#include <latan/Math.hpp>
#include <latan/includes.hpp>

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

// Exponential and logarithmic functions
DEF_STD_FUNC_1ARG(exp)
DEF_STD_FUNC_1ARG(log)

// Power functions
DEF_STD_FUNC_2ARG(pow)
DEF_STD_FUNC_1ARG(sqrt)

// Rounding and remainder functions
DEF_STD_FUNC_1ARG(ceil)
DEF_STD_FUNC_1ARG(floor)
DEF_STD_FUNC_2ARG(fmod)

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
    // Exponential and logarithmic functions
    ADD_FUNC(exp);
    ADD_FUNC(log);
    // Power functions
    ADD_FUNC(pow);
    ADD_FUNC(sqrt);
    // Rounding and remainder functions
    ADD_FUNC(ceil);
    ADD_FUNC(floor);
    ADD_FUNC(fmod);
    // Minimum, maximum, difference functions
    ADD_FUNC(fdim);
    ADD_FUNC(fmax);
    ADD_FUNC(fmin);
    // Absolute value
    ADD_FUNC(fabs);
}
