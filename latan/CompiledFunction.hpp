/*
 * CompiledFunction.hpp, part of LatAnalyze 3
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

#ifndef Latan_CompiledFunction_hpp_
#define	Latan_CompiledFunction_hpp_

#include <latan/Global.hpp>
#include <latan/Function.hpp>
#include <latan/MathInterpreter.hpp>
#include <stack>
#include <vector>
#include <cstdarg>

BEGIN_NAMESPACE

/******************************************************************************
 *                      Compiled double function class                        *
 ******************************************************************************/
class CompiledDoubleFunction: public DoubleFunction
{
public:
    // constructors
    explicit CompiledDoubleFunction(const unsigned nArg);
    explicit CompiledDoubleFunction(const unsigned nArg,
                                    const std::string &code);
    // destructor
    virtual ~CompiledDoubleFunction(void);
    // access
    void setCode(const std::string &code);
    // function call
    virtual double evaluate(const std::vector<double> &arg) const;
    // IO
    friend std::ostream &operator<<(std::ostream &out,
                                    CompiledDoubleFunction &f);
private:
    MathInterpreter* interpreter_;
    RunContext*      context_;
};

END_NAMESPACE

#endif // Latan_CompiledFunction_hpp_
