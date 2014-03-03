/*
 * CompiledFunction.cpp, part of LatAnalyze 3
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

#include <latan/CompiledFunction.hpp>
#include <latan/Math.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                   Compiled double function implementation                  *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
CompiledDoubleFunction::CompiledDoubleFunction(const unsigned nArg)
: DoubleFunction(nArg, nullptr)
{}

CompiledDoubleFunction::CompiledDoubleFunction(const unsigned nArg,
                                               const string &code)
: DoubleFunction(nArg, nullptr)
{
    setCode(code);
}

// access //////////////////////////////////////////////////////////////////////
void CompiledDoubleFunction::setCode(const string &code)
{
    interpreter_.reset(new MathInterpreter(code));
    context_.reset(new RunContext);
    StdMath::addStdMathFunc(context_->fTable);
}

// function call ///////////////////////////////////////////////////////////////
double CompiledDoubleFunction::operator()(const double *arg) const
{
    double result;
    
    for (Index i = 0; i < getNArg(); ++i)
    {
        context_->vTable["x_" + strFrom(i)] = arg[i];
    }
    (*interpreter_)(*context_);
    if (!context_->dStack.empty())
    {
        result = context_->dStack.top();
        context_->dStack.pop();
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
    f.interpreter_->compile();
    out << *(f.interpreter_);
    
    return out;
}
