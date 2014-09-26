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
    varAddress_.reset(new std::vector<unsigned int>);
    parAddress_.reset(new std::vector<unsigned int>);
    isCompiled_.reset(new bool(false));
}

// compile /////////////////////////////////////////////////////////////////////
void CompiledDoubleModel::compile(void) const
{
    if (!*isCompiled_)
    {
        varAddress_->clear();
        parAddress_->clear();
        for (Index i = 0; i < getNArg(); ++i)
        {
            varAddress_->push_back(context_->addVariable("x_" + strFrom(i)));
        }
        for (Index j = 0; j < getNPar(); ++j)
        {
            parAddress_->push_back(context_->addVariable("p_" + strFrom(j)));
        }
        interpreter_->compile(*(context_));
        *isCompiled_ = true;
    }
}

// function call ///////////////////////////////////////////////////////////////
double CompiledDoubleModel::operator()(const double *arg,
                                       const double *par) const
{
    double result;
    
    compile();
    for (unsigned int i = 0; i < getNArg(); ++i)
    {
        context_->setVariable((*varAddress_)[i], arg[i]);
    }
    for (unsigned int j = 0; j < getNPar(); ++j)
    {
        context_->setVariable((*parAddress_)[j], par[j]);
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
ostream & Latan::operator<<(std::ostream &out, CompiledDoubleModel &m)
{
    m.compile();
    out << *(m.interpreter_);
    
    return out;
}
