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
#include <vector>

BEGIN_NAMESPACE

/******************************************************************************
 *                             Standard C functions                           *
 ******************************************************************************/
#define STDMATH_NAMESPACE StdMath

#define DECL_STD_FUNC(name, funcName) \
class name##Function: public DoubleFunction\
{\
public:\
    name##Function(void);\
    virtual ~name##Function(void);\
    using DoubleFunction::operator();\
    virtual double operator()(std::vector<double> &arg);\
};\
namespace STDMATH_NAMESPACE\
{\
    extern name##Function funcName;\
}

// Trigonometric functions
DECL_STD_FUNC(Cos,   cos)
DECL_STD_FUNC(Sin,   sin)
DECL_STD_FUNC(Tan,   tan)
DECL_STD_FUNC(ACos,  acos)
DECL_STD_FUNC(ASin,  asin)
DECL_STD_FUNC(ATan,  atan)
DECL_STD_FUNC(ATan2, atan2)

// Hyperbolic functions
DECL_STD_FUNC(Cosh, cosh)
DECL_STD_FUNC(Sinh, sinh)
DECL_STD_FUNC(Tanh, tanh)

// Exponential and logarithmic functions
DECL_STD_FUNC(Exp, exp)
DECL_STD_FUNC(Log, log)

// Power functions
DECL_STD_FUNC(Pow,  pow)
DECL_STD_FUNC(Sqrt, sqrt)

// Rounding and remainder functions
DECL_STD_FUNC(Ceil,  ceil)
DECL_STD_FUNC(Floor, floor)
DECL_STD_FUNC(FMod,  fmod)

// Minimum, maximum, difference functions
DECL_STD_FUNC(FDim, fdim)
DECL_STD_FUNC(FMax, fmax)
DECL_STD_FUNC(FMin, fmin)

// Absolute value
DECL_STD_FUNC(Abs, abs)

// Add standard math functions to a table for the math compiler
namespace STDMATH_NAMESPACE
{
    void addStdMathFunc(FunctionTable &fTable);
}

END_NAMESPACE

#endif // Latan_Math_hpp_
