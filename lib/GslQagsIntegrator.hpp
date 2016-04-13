/*
 * GslQagsIntegrator.hpp, part of LatAnalyze 3
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

#ifndef Latan_GslQagsIntegrator_hpp_
#define Latan_GslQagsIntegrator_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Integrator.hpp>
#include <gsl/gsl_integration.h>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *       GSL general quadrature adaptive integration with singularities       *
 ******************************************************************************/

class GslQagsIntegrator: public Integrator
{
public:
    static const     unsigned int defaultLimit = 1000;
    static constexpr double       defaultPrec  = 1.0e-7;
public:
    // constructor
    GslQagsIntegrator(const unsigned int limit = defaultLimit,
                      const double precision = defaultPrec);
    // destructor
    virtual ~GslQagsIntegrator(void);
    // integral calculation
    virtual double operator()(const DoubleFunction &f, const double xMin,
                              const double xMax);
    // get last error
    double getLastError(void) const;
private:
    unsigned int              limit_;
    double                    precision_, error_;
    gsl_integration_workspace *workspace_;
};

END_LATAN_NAMESPACE

#endif // Latan_GslQagsIntegrator_hpp_
