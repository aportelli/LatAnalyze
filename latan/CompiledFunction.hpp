#ifndef LATAN_COMPILED_FUNCTION_HPP_
#define	LATAN_COMPILED_FUNCTION_HPP_

#include <latan/Global.hpp>
#include <latan/Function.hpp>
#include <latan/MathInterpreter.hpp>
#include <stack>
#include <vector>
#include <cstdarg>

LATAN_BEGIN_CPPDECL

/******************************************************************************
 *                      Compiled double function class                        *
 ******************************************************************************/
class CompiledDoubleFunction: public DoubleFunction
{
public:
    // constructor/destructor
    explicit CompiledDoubleFunction(const unsigned nArg);
    explicit CompiledDoubleFunction(const unsigned nArg,
                                    const std::string &code);
    virtual ~CompiledDoubleFunction(void);
    // access
    void setCode(const std::string &code);
    // function call
    using DoubleFunction::operator();
    virtual double operator()(std::vector<double> &arg);
    // IO
    friend std::ostream &operator<<(std::ostream &out,
                                    CompiledDoubleFunction &f);
private:
    MathInterpreter interpreter_;
    RunContext      context_;
};

LATAN_END_CPPDECL

#endif
