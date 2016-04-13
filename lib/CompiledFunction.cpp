/*
 * CompiledFunction.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
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

#include <LatAnalyze/CompiledFunction.hpp>
#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                   Compiled double function implementation                  *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
CompiledDoubleFunction::CompiledDoubleFunction(const Index nArg)
: nArg_(nArg)
{}

CompiledDoubleFunction::CompiledDoubleFunction(const string &code,
                                               const Index nArg)
: CompiledDoubleFunction(nArg)
{
    setCode(code);
}

// access //////////////////////////////////////////////////////////////////////
string CompiledDoubleFunction::getCode(void)
{
    return code_;
}

void CompiledDoubleFunction::setCode(const string &code)
{
    code_ = code;
    interpreter_.reset(new MathInterpreter(code));
    context_.reset(new RunContext);
    varAddress_.reset(new std::vector<unsigned int>);
    isCompiled_.reset(new bool(false));
}

// compile /////////////////////////////////////////////////////////////////////
void CompiledDoubleFunction::compile(void) const
{
    if (!*isCompiled_)
    {
        varAddress_->clear();
        for (Index i = 0; i < nArg_; ++i)
        {
            varAddress_->push_back(context_->addVariable("x_" + strFrom(i)));
        }
        interpreter_->compile(*(context_));
        *isCompiled_ = true;
    }
}

// function call ///////////////////////////////////////////////////////////////
double CompiledDoubleFunction::operator()(const double *arg) const
{
    double result;
    
    compile();
    for (unsigned int i = 0; i < nArg_; ++i)
    {
        context_->setVariable((*varAddress_)[i], arg[i]);
    }
    (*interpreter_)(*context_);
    if (!context_->stack().empty())
    {
        result = context_->stack().top();
        context_->stack().pop();
    }
    else
    {
        result = 0.0;
        LATAN_ERROR(Program, "program execution resulted in an empty stack");
    }
    
    return result;
}

// IO //////////////////////////////////////////////////////////////////////////
ostream & Latan::operator<<(ostream &out, CompiledDoubleFunction &f)
{
    f.compile();
    out << *(f.interpreter_);
    
    return out;
}

// DoubleFunction factory //////////////////////////////////////////////////////
DoubleFunction CompiledDoubleFunction::makeFunction(const bool makeHardCopy)
                                                    const
{
    DoubleFunction res;

    if (makeHardCopy)
    {
        CompiledDoubleFunction copy(*this);

        res.setFunction([copy](const double *p){return copy(p);}, nArg_);
    }
    else
    {
        res.setFunction([this](const double *p){return (*this)(p);}, nArg_);
    }
    
    return res;
}

DoubleFunction Latan::compile(const string code, const Index nArg)
{
    CompiledDoubleFunction compiledFunc(code, nArg);

    return compiledFunc.makeFunction();
}
