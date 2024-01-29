/*
 * CompiledFunction.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#ifndef Latan_CompiledFunction_hpp_
#define	Latan_CompiledFunction_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Functional/Function.hpp>
#include <LatAnalyze/Core/MathInterpreter.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                      compiled double function class                        *
 ******************************************************************************/
class CompiledDoubleFunction: public DoubleFunctionFactory
{
public:
    // constructors
    explicit CompiledDoubleFunction(const Index nArg);
    CompiledDoubleFunction(const std::string &code, const Index nArg);
    // destructor
    virtual ~CompiledDoubleFunction(void) = default;
    // access
    std::string getCode(void);
    void        setCode(const std::string &code);
    // function call
    double operator()(const double *arg) const;
    // IO
    friend std::ostream & operator<<(std::ostream &out,
                                     CompiledDoubleFunction &f);
    // factory
    virtual DoubleFunction makeFunction(const bool makeHardCopy = true) const;
private:

    // compile
    void compile(void) const;
private:
    Index                                      nArg_;
    std::string                                code_;
    std::shared_ptr<MathInterpreter>           interpreter_;
    std::shared_ptr<RunContext>                context_;
    std::shared_ptr<std::vector<unsigned int>> varAddress_;
    std::shared_ptr<bool>                      isCompiled_;
};

std::ostream & operator<<(std::ostream &out, CompiledDoubleFunction &f);

// DoubleFunction factory
DoubleFunction compile(const std::string code, const Index nArg);

END_LATAN_NAMESPACE

#endif // Latan_CompiledFunction_hpp_
