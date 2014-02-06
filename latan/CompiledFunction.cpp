#include <latan/CompiledFunction.hpp>
#include <latan/Math.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                   Compiled double function implementation                  *
 ******************************************************************************/
// constructor/destructor //////////////////////////////////////////////////////
CompiledDoubleFunction::CompiledDoubleFunction(const unsigned nArg)
: DoubleFunction(nArg)
{}

CompiledDoubleFunction::CompiledDoubleFunction(const unsigned nArg,
                                               const string &code)
: DoubleFunction(nArg)
{
    setCode(code);
}

CompiledDoubleFunction::~CompiledDoubleFunction(void)
{}

// access //////////////////////////////////////////////////////////////////////
void CompiledDoubleFunction::setCode(const string &code)
{
    interpreter_.setCode(code);
    StdMath::addStdMathFunc(context_.fTable);
}

// function call ///////////////////////////////////////////////////////////////
double CompiledDoubleFunction::operator()(vector<double> &arg)
{
    double result;
    
    if (arg.size() != getNArg())
    {
        LATAN_ERROR(Size, "function argument vector has a wrong size (got " +
                    strFrom(arg.size()) + ", expected " + strFrom(getNArg()) +
                    ")");
    }
    for (unsigned int i = 0; i < getNArg(); ++i)
    {
        context_.vTable["x_" + strFrom(i)] = arg[i];
    }
    interpreter_(context_);
    if (!context_.dStack.empty())
    {
        result = context_.dStack.top();
        context_.dStack.pop();
    }
    else
    {
        result = 0.0;
        LATAN_ERROR(Program, "program execution resulted in an empty stack");
    }
    
    return result;
}

// IO //////////////////////////////////////////////////////////////////////////
ostream &Latan::operator<<(ostream &out, CompiledDoubleFunction &f)
{
    out << f.interpreter_;
    
    return out;
}
