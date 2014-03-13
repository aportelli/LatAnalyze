/*
 * CompiledModel.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/Math.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                 CompiledDoubleModel implementation                         *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
CompiledDoubleModel::CompiledDoubleModel(const unsigned nArg,
                                         const unsigned nPar)
: DoubleModel(nArg, nPar, nullptr)
{}

CompiledDoubleModel::CompiledDoubleModel(const unsigned nArg,
                                         const unsigned nPar,
                                         const string &code)
: CompiledDoubleModel(nArg, nPar)
{
    setCode(code);
}

// access //////////////////////////////////////////////////////////////////////
void CompiledDoubleModel::setCode(const std::string &code)
{
    interpreter_.reset(new MathInterpreter(code));
    context_.reset(new RunContext);
    StdMath::addStdMathFunc(context_->fTable);
}

// function call ///////////////////////////////////////////////////////////////
double CompiledDoubleModel::operator()(const double *arg,
                                       const double *par) const
{
    double result;
    
    for (Index i = 0; i < getNArg(); ++i)
    {
        context_->vTable["x_" + strFrom(i)] = arg[i];
    }
    for (Index j = 0; j < getNPar(); ++j)
    {
        context_->vTable["p_" + strFrom(j)] = par[j];
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
ostream & Latan::operator<<(std::ostream &out, CompiledDoubleModel &m)
{
    m.interpreter_->compile();
    out << *(m.interpreter_);
    
    return out;
}
