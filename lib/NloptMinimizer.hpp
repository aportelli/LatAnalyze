/*
 * NloptMinimizer.hpp, part of LatAnalyze 3
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

#ifndef Latan_NloptMinimizer_hpp_
#define Latan_NloptMinimizer_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Derivative.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Minimizer.hpp>
#include <nlopt.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                      interface to NLOpt minimizers                         *
 * ( http://ab-initio.mit.edu/wiki/index.php/NLopt )                          *
 * -------------------------------------------------------------------------- *
 * cf. http://ab-initio.mit.edu/wiki/index.php/NLopt_Algorithms for algorithm *
 * references and naming conventions                                          *
 ******************************************************************************/
class NloptMinimizer: public Minimizer
{
public:
    typedef nlopt::algorithm Algorithm;
private:
    struct NloptFuncData
    {
        const DoubleFunction *f{nullptr};
        unsigned int         evalCount{0};
    };
public:
    // constructor
    NloptMinimizer(const Algorithm algorithm = defaultAlg_);
    explicit NloptMinimizer(const Index dim,
                            const Algorithm algorithm = defaultAlg_);
    // destructor
    virtual ~NloptMinimizer(void) = default;
    // access
    Algorithm getAlgorithm(void) const;
    void      setAlgorithm(const Algorithm algorithm);
    // minimization
    virtual const DVec & operator()(const DoubleFunction &f);
private:
    // NLopt return code parser
    static std::string returnMessage(const nlopt::result status);
    // NLopt function wrapper
    static double funcWrapper(unsigned int n, const double *arg,
                              double *grad , void *vdata);
private:
    Algorithm                  algorithm_;
    static constexpr Algorithm defaultAlg_ = Algorithm::LN_NELDERMEAD;
};

END_LATAN_NAMESPACE

#endif // Latan_NloptMinimizer_hpp_
