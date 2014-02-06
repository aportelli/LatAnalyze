#ifndef LATAN_MATH_HPP_
#define	LATAN_MATH_HPP_

#include <latan/Global.hpp>
#include <latan/Function.hpp>
#include <latan/MathInterpreter.hpp>
#include <vector>

LATAN_BEGIN_CPPDECL

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

LATAN_END_CPPDECL

#endif
