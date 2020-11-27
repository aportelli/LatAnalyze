/*
 * GslMinimizer.hpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Latan_GslMinimizer_hpp_
#define Latan_GslMinimizer_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Numerical/Derivative.hpp>
#include <LatAnalyze/Functional/Function.hpp>
#include <LatAnalyze/Numerical/Minimizer.hpp>
#include <gsl/gsl_vector.h>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                    interface to the GSL minimizers                         *
 ******************************************************************************/
class GslMinimizer: public Minimizer
{
public:
    enum class Algorithm
    {
        cgFR       = 1,
        cgPR       = 2,
        bfgs       = 3,
        bfgs2      = 4,
        steepDesc  = 5,
        lastDerAlg = 5,
        simplex    = 6,
        simplex2   = 7,
        simplex2R  = 8
    };
private:
    struct GslFuncData
    {
        const DoubleFunction *f{nullptr};
        Derivative           *d{nullptr};
        unsigned int         evalCount{0};
    };
public:
    // constructor
    explicit GslMinimizer(const Algorithm algorithm = defaultAlg_);
    // destructor
    virtual ~GslMinimizer(void) = default;
    // access
    Algorithm    getAlgorithm(void) const;
    void         setAlgorithm(const Algorithm algorithm);
    virtual bool supportLimits(void) const;
    // minimization
    virtual const DVec & operator()(const DoubleFunction &f);
private:
    // test
    static bool isDerAlgorithm(const Algorithm algorithm);
    // function wrappers
    static double fWrapper(const gsl_vector  *x, void * params);
    static void   dfWrapper(const gsl_vector *x, void * params,
                            gsl_vector * df);
    static void   fdfWrapper(const gsl_vector *x, void *params, double *f,
                             gsl_vector * df);
    // algorithm names
    std::string getAlgorithmName(const Algorithm algorithm);
private:
    Algorithm                  algorithm_;
    static constexpr Algorithm defaultAlg_ = Algorithm::simplex2;
    CentralDerivative          der_;
};

END_LATAN_NAMESPACE

#endif // Latan_GslMinimizer_hpp_
