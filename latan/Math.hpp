/*
 * Math.hpp, part of LatAnalyze 3
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

#ifndef Latan_Math_hpp_
#define	Latan_Math_hpp_

#include <latan/Global.hpp>
#include <latan/Function.hpp>
#include <latan/MathInterpreter.hpp>

BEGIN_NAMESPACE

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

// Exponential and logarithmic functions
DECL_STD_FUNC(exp)
DECL_STD_FUNC(log)

// Power functions
DECL_STD_FUNC(pow)
DECL_STD_FUNC(sqrt)

// Rounding and remainder functions
DECL_STD_FUNC(ceil)
DECL_STD_FUNC(floor)
DECL_STD_FUNC(fmod)

// Minimum, maximum, difference functions
DECL_STD_FUNC(fdim)
DECL_STD_FUNC(fmax)
DECL_STD_FUNC(fmin)

// Absolute value
DECL_STD_FUNC(fabs)

// Add standard math functions to a table for the math compiler
namespace STDMATH_NAMESPACE
{
    void addStdMathFunc(FunctionTable &fTable);
}

END_NAMESPACE

#endif // Latan_Math_hpp_
