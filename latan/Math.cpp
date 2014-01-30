#include <latan/Math.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                             Standard C functions                           *
 ******************************************************************************/

#define DEF_STD_FUNC_1ARG(name, funcName) \
name##Function::name##Function(void): DoubleFunction(1) {}\
name##Function::~name##Function(void) {}\
double name##Function::operator()(std::vector<double> &arg)\
{\
    return funcName(arg[0]);\
}\
name##Function STDMATH_NAMESPACE::funcName;

#define DEF_STD_FUNC_2ARG(name, funcName) \
name##Function::name##Function(void): DoubleFunction(2) {}\
name##Function::~name##Function(void) {}\
double name##Function::operator()(std::vector<double> &arg)\
{\
    return funcName(arg[0], arg[1]);\
}\
name##Function STDMATH_NAMESPACE::funcName;

// Trigonometric functions
DEF_STD_FUNC_1ARG(Cos,   cos)
DEF_STD_FUNC_1ARG(Sin,   sin)
DEF_STD_FUNC_1ARG(Tan,   tan)
DEF_STD_FUNC_1ARG(ACos,  acos)
DEF_STD_FUNC_1ARG(ASin,  asin)
DEF_STD_FUNC_1ARG(ATan,  atan)
DEF_STD_FUNC_2ARG(ATan2, atan2)

// Hyperbolic functions
DEF_STD_FUNC_1ARG(Cosh, cosh)
DEF_STD_FUNC_1ARG(Sinh, sinh)
DEF_STD_FUNC_1ARG(Tanh, tanh)

// Exponential and logarithmic functions
DEF_STD_FUNC_1ARG(Exp, exp)
DEF_STD_FUNC_1ARG(Log, log)

// Power functions
DEF_STD_FUNC_2ARG(Pow,  pow)
DEF_STD_FUNC_1ARG(Sqrt, sqrt)

// Rounding and remainder functions
DEF_STD_FUNC_1ARG(Ceil,  ceil)
DEF_STD_FUNC_1ARG(Floor, floor)
DEF_STD_FUNC_2ARG(FMod,  fmod)

// Minimum, maximum, difference functions
DEF_STD_FUNC_2ARG(FDim, fdim)
DEF_STD_FUNC_2ARG(FMax, fmax)
DEF_STD_FUNC_2ARG(FMin, fmin)

// Absolute value
DEF_STD_FUNC_1ARG(Abs, abs)

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
    ADD_FUNC(abs);
}
