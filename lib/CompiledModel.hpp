/*
 * CompiledModel.hpp, part of LatAnalyze 3
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

#ifndef Latan_CompiledModel_hpp_
#define Latan_CompiledModel_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Model.hpp>
#include <LatAnalyze/MathInterpreter.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                     compiled double model class                            *
 ******************************************************************************/
class CompiledDoubleModel: public DoubleModelFactory
{
public:
    // constructor
    CompiledDoubleModel(const Index nArg, const Index nPar);
    CompiledDoubleModel(const std::string &code, const Index nArg,
                        const Index nPar);
    // destructor
    virtual ~CompiledDoubleModel(void) = default;
    // access
    std::string getCode(void);
    void        setCode(const std::string &code);
    // function call
    double operator()(const double *arg, const double *par) const;
    // IO
    friend std::ostream & operator<<(std::ostream &out,
                                     CompiledDoubleModel &f);
    // factory
    DoubleModel makeModel(const bool makeHardCopy = true) const;
private:
    // compile
    void compile(void) const;
private:
    Index                                      nArg_, nPar_;
    std::string                                code_;
    std::shared_ptr<MathInterpreter>           interpreter_;
    std::shared_ptr<RunContext>                context_;
    std::shared_ptr<std::vector<unsigned int>> varAddress_, parAddress_;
    std::shared_ptr<bool>                      isCompiled_;
};

std::ostream & operator<<(std::ostream &out, CompiledDoubleModel &f);

// DoubleModel factory
DoubleModel compile(const std::string &code, const Index nArg,
                    const Index nPar);

END_LATAN_NAMESPACE

#endif // Latan_CompiledModel_hpp_
