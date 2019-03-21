/*
 * CompiledModel.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Functional/CompiledModel.hpp>
#include <LatAnalyze/Core/Math.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                 CompiledDoubleModel implementation                         *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
CompiledDoubleModel::CompiledDoubleModel(const Index nArg, const Index nPar)
: nArg_(nArg)
, nPar_(nPar)
{}

CompiledDoubleModel::CompiledDoubleModel(const string &code, const Index nArg,
                                         const Index nPar)
: CompiledDoubleModel(nArg, nPar)
{
    setCode(code);
}

// access //////////////////////////////////////////////////////////////////////
string CompiledDoubleModel::getCode(void)
{
    return code_;
}

void CompiledDoubleModel::setCode(const std::string &code)
{
    code_ = code;
    interpreter_.reset(new MathInterpreter(code_));
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
        for (Index i = 0; i < nArg_; ++i)
        {
            varAddress_->push_back(context_->addVariable("x_" + strFrom(i)));
        }
        for (Index j = 0; j < nPar_; ++j)
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
    for (unsigned int i = 0; i < nArg_; ++i)
    {
        context_->setVariable((*varAddress_)[i], arg[i]);
    }
    for (unsigned int j = 0; j < nPar_; ++j)
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

// DoubleModel factory /////////////////////////////////////////////////////////
DoubleModel CompiledDoubleModel::makeModel(const bool makeHardCopy) const
{
    DoubleModel res;

    if (makeHardCopy)
    {
        CompiledDoubleModel copy(*this);

        res.setFunction([copy](const double *x, const double *p)
                        {return copy(x, p);}, nArg_, nPar_);
    }
    else
    {
        res.setFunction([this](const double *x, const double *p)
                        {return (*this)(x, p);}, nArg_, nPar_);
    }

    return res;
}

DoubleModel Latan::compile(const std::string &code, const Index nArg,
                           const Index nPar)
{
    CompiledDoubleModel compiledModel(code, nArg, nPar);

    return compiledModel.makeModel();
}
