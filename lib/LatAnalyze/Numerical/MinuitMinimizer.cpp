/*
 * MinuitMinimizer.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Numerical/MinuitMinimizer.hpp>
#include <LatAnalyze/includes.hpp>

// forward declaration necessary in the ROOT-based version of Minuit2
namespace ROOT
{
    namespace Fit
    {
        class ParameterSettings;
    };
};

// macros necessary in the ROOT-based version of Minuit2
#ifndef ROOT_Math_VecTypes
#define ROOT_Math_VecTypes
#endif
#ifndef MATHCORE_STANDALONE
#define MATHCORE_STANDALONE
#endif

#include <Minuit2/Minuit2Minimizer.h>
#include <Math/Functor.h>

using namespace std;
using namespace Latan;

static constexpr double initErr = 0.1;

/******************************************************************************
 *                    MinuitMinimizer implementation                          *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
MinuitMinimizer::MinuitMinimizer(const Algorithm algorithm)
{
    setAlgorithm(algorithm);
}

// access //////////////////////////////////////////////////////////////////////
MinuitMinimizer::Algorithm MinuitMinimizer::getAlgorithm(void) const
{
    return algorithm_;
}

void MinuitMinimizer::setAlgorithm(const Algorithm algorithm)
{
    algorithm_ = algorithm;
}

bool MinuitMinimizer::supportLimits(void) const
{
    return true;
}

// minimization ////////////////////////////////////////////////////////////////
const DVec & MinuitMinimizer::operator()(const DoubleFunction &f)
{
    using namespace ROOT;
    using namespace Minuit2;
    
    DVec           &x = getState();
    int            printLevel = 0;
    EMinimizerType minuitAlg  = kCombined;
    double         prec = getPrecision();
    
    // convert Latan parameters to Minuit parameters
    switch (getVerbosity())
    {
        case Verbosity::Silent:
            printLevel = 0;
            break;
        case Verbosity::Normal:
            printLevel = 2;
            break;
        case Verbosity::Debug:
            printLevel = 3;
            break;
    }
    // The factor of 0.002 here is to compensate the dirty hack in Minuit
    // source used to match the C++ and F77 versions
    // (cf. VariableMetricBuilder.cxx)
    switch (getAlgorithm())
    {
        case Algorithm::migrad:
            minuitAlg  = kMigrad;
            prec      /= 0.002;
            break;
        case Algorithm::simplex:
            minuitAlg = kSimplex;
            break;
        case Algorithm::combined:
            minuitAlg  = kCombined;
            prec      /= 0.002;
            break;
    }
    
    // resize minimizer state to match function number of arguments
    if (f.getNArg() != x.size())
    {
        resize(f.getNArg());
    }
    
    // create and set minimizer
    Minuit2Minimizer min(minuitAlg);
    
    min.SetStrategy(2);
    min.SetMaxFunctionCalls(getMaxIteration());
    min.SetTolerance(prec);
    min.SetPrintLevel(printLevel);
    
    // set function and variables
    Math::Functor minuitF(f, x.size());
    string        name;
    double        val, step;
    
    min.SetFunction(minuitF);
    for (Index i = 0; i < x.size(); ++i)
    {
        name = f.varName().getName(i);
        val  = x(i);
        step = (fabs(x(i)) != 0.) ? initErr*fabs(x(i)) : 1.;
        if (hasHighLimit(i) and !hasLowLimit(i))
        {
            min.SetUpperLimitedVariable(i, name, val, step, getHighLimit(i));
        }
        else if (!hasHighLimit(i) and hasLowLimit(i))
        {
            min.SetLowerLimitedVariable(i, name, val, step, getLowLimit(i));
        }
        else if (hasHighLimit(i) and hasLowLimit(i))
        {
            min.SetLimitedVariable(i, name, val, step, getLowLimit(i),
                                   getHighLimit(i));
        }
        else
        {
            min.SetVariable(i, name, val, step);
        }
    }
    
    // minimize
    int          status;
    unsigned int n = 0;
    
    do
    {
        if (getVerbosity() >= Verbosity::Normal)
        {
            cout << "========== Minuit minimization, pass #" << n + 1 << endl;
        }
        min.Minimize();
        status = min.Status();
        n++;
    } while ((status >= 2) and (n < getMaxPass()));
    if (getVerbosity() >= Verbosity::Normal)
    {
        cout << "=================================================" << endl;
    }
    switch (status)
    {
        case 1:
            // covariance matrix was made positive, the minimum is still good
            // it just means that Minuit error analysis is inaccurate
            break;
        case 2:
            LATAN_WARNING("invalid minimum: Hesse analysis is not valid");
            break;
        case 3:
            LATAN_WARNING("invalid minimum: requested precision not reached");
            break;
        case 4:
            LATAN_WARNING("invalid minimum: iteration limit reached");
            break;
    }
    
    // save and return result
    for (Index i = 0; i < x.size(); ++i)
    {
        x(i) = min.X()[i];
    }
    
    return x;
}
