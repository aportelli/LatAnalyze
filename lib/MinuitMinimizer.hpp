/*
 * MinuitMinimizer.hpp, part of LatAnalyze 3
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

#ifndef Latan_MinuitMinimizer_hpp_
#define Latan_MinuitMinimizer_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Minimizer.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                  interface to CERN Minuit minimizer                        *
 * ( http://www.cern.ch/minuit )                                              *
 ******************************************************************************/
class MinuitMinimizer: public Minimizer
{
public:
    enum class Algorithm
    {
        Migrad   = 1,
        Simplex  = 2,
        Combined = 3
    };
public:
    // constructor
    MinuitMinimizer(const Algorithm algorithm = defaultAlg_);
    explicit MinuitMinimizer(const Index dim,
                             const Algorithm algorithm = defaultAlg_);
    // destructor
    virtual ~MinuitMinimizer(void) = default;
    // access
    Algorithm getAlgorithm(void) const;
    void      setAlgorithm(const Algorithm algorithm);
    // minimization
    virtual const DVec & operator()(const DoubleFunction &f);
private:
    Algorithm                  algorithm_;
    static constexpr Algorithm defaultAlg_ = Algorithm::Combined;
};

END_LATAN_NAMESPACE

#endif // Latan_MinuitMinimizer_hpp_
