/*
 * GslQagsIntegrator.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Numerical/GslQagsIntegrator.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Core/Math.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                    GslQagIntegrator implementation                         *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
GslQagsIntegrator::GslQagsIntegrator(const unsigned int limit,
                                     const double precision)
: limit_(limit)
, precision_(precision)
{
    workspace_ = gsl_integration_workspace_alloc(limit);
}

// destructor //////////////////////////////////////////////////////////////////
GslQagsIntegrator::~GslQagsIntegrator(void)
{
    gsl_integration_workspace_free(workspace_);
}

// integral calculation ////////////////////////////////////////////////////////
double GslQagsIntegrator::operator()(const DoubleFunction &f, const double xMin,
                                     const double xMax)
{
    double (*fWrap)(double, void *) = [](double x, void *fPt)->double
    {
        return (*static_cast<DoubleFunction *>(fPt))(&x);
    };
    
    gsl_function gslF;
    double       result;
    
    gslF.function = fWrap;
    gslF.params   = reinterpret_cast<void *>(&const_cast<DoubleFunction &>(f));
    if ((xMin > -Math::inf) and (xMax < Math::inf))
    {
        gsl_integration_qags(&gslF, xMin, xMax, 0.0, precision_, limit_,
                             workspace_, &result, &error_);
    }
    else if (xMax < Math::inf)
    {
        gsl_integration_qagil(&gslF, xMax, 0.0, precision_, limit_,
                              workspace_, &result, &error_);
    }
    else if (xMin > -Math::inf)
    {
        gsl_integration_qagiu(&gslF, xMin, 0.0, precision_, limit_,
                              workspace_, &result, &error_);
    }
    else
    {
        gsl_integration_qagi(&gslF, 0.0, precision_, limit_,
                             workspace_, &result, &error_);
    }
    
    return result;
}

// get last error //////////////////////////////////////////////////////////////
double GslQagsIntegrator::getLastError(void) const
{
    return error_;
}
