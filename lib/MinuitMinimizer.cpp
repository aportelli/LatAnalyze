/*
 * MinuitMinimizer.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/MinuitMinimizer.hpp>
#include <LatAnalyze/includes.hpp>

#pragma GCC diagnostic ignored "-Wdeprecated-register"
#pragma GCC diagnostic ignored "-Wconversion"

#include <Minuit2/FCNBase.h>
#include <Minuit2/FunctionMinimum.h>
#include <Minuit2/MnMigrad.h>
#include <Minuit2/MnPrint.h>
#include <Minuit2/MnPlot.h>
#include <Minuit2/MnScan.h>
#include <Minuit2/MnSimplex.h>
#include <Minuit2/ScanMinimizer.h>
#include <Minuit2/SimplexMinimizer.h>
#include <Minuit2/VariableMetricMinimizer.h>

#pragma GCC diagnostic warning "-Wdeprecated-register"
#pragma GCC diagnostic warning "-Wconversion"

using namespace std;
using namespace ROOT;
using namespace Minuit2;
using namespace Latan;

static constexpr double       initErr = 0.5;
static constexpr unsigned int maxTry  = 10u;

/******************************************************************************
 *                    MinuitMinimizer implementation                          *
 ******************************************************************************/
// MinuitFunction constructor //////////////////////////////////////////////////
MinuitMinimizer::MinuitFunction::MinuitFunction(const DoubleFunction &f)
: f_(&f)
{}

// MinuitFunction minuit members ///////////////////////////////////////////////
double MinuitMinimizer::MinuitFunction::operator()
    (const vector<double> &x) const
{
    return (*f_)(x);
}

double MinuitMinimizer::MinuitFunction::Up(void) const
{
    return 1.;
}

// constructor /////////////////////////////////////////////////////////////////
MinuitMinimizer::MinuitMinimizer(const Index dim)
: Minimizer(dim)
{}

// access //////////////////////////////////////////////////////////////////////
double MinuitMinimizer::getPrecision(void) const
{
    LATAN_ERROR(Implementation,
                "Minuit minimizer precision cannot be accessed");
    
    return 0.;
}

void MinuitMinimizer::setPrecision(const double precision __unused)
{
    LATAN_ERROR(Implementation,
                "Minuit minimizer precision cannot be accessed");
}

// minimization ////////////////////////////////////////////////////////////////
const DVec & MinuitMinimizer::operator()(const DoubleFunction &f)
{
    DVec &x = getState();
    Verbosity verbosity = getVerbosity();
    
    if (f.getNArg() != x.size())
    {
        LATAN_ERROR(Size, "function to minimize number of arguments mismatch");
    }
    
    // set parameters
    MinuitFunction   minuitF(f);
    MnUserParameters parameters;
    
    for (Index i = 0; i < x.size(); ++i)
    {
        parameters.Add("x_" + strFrom(i), x(i), initErr*fabs(x(i)));
        if (hasLowLimit(i)&&hasHighLimit(i))
        {
            parameters.SetLimits(static_cast<unsigned int>(i),
                                 getLowLimit(i), getHighLimit(i));
        }
        else if (hasLowLimit(i))
        {
            parameters.SetLowerLimit(static_cast<unsigned int>(i),
                                     getLowLimit(i));
        }
        else if (hasHighLimit(i))
        {
            parameters.SetUpperLimit(static_cast<unsigned int>(i),
                                     getHighLimit(i));
        }
    }
    
    // pre-minimization
    MnSimplex       preMinimizer(minuitF, parameters, 1);
    FunctionMinimum preMin = preMinimizer();
    
    if (verbosity >= Verbosity::Debug)
    {
        cout << "-- MINUIT pre-minimization -----------------------------";
        cout << preMin;
        cout << "--------------------------------------------------------";
        cout << endl;
    }
    for (unsigned int i = 0; i < x.size(); ++i)
    {
        parameters.SetValue(i, preMin.UserParameters().Value(i));
        parameters.SetError(i, 2.*preMin.UserParameters().Error(i));
    }
    
    // minimization and output
    MnMigrad        minimizer(minuitF, parameters, 2);
    unsigned int    iTry = 0;
    FunctionMinimum min = minimizer();

    while ((!min.IsValid())&&(iTry < maxTry))
    {
        min = minimizer();
        iTry++;
    }
    if (!min.IsValid())
    {
        LATAN_WARNING("MINUIT library reported that minimization result is not valid");
    }
    for (unsigned int i = 0; i < x.size(); ++i)
    {
        x(i) = min.UserParameters().Value(i);
    }
    if (verbosity >= Verbosity::Normal)
    {
        cout << "-- MINUIT minimization ---------------------------------";
        cout << min;
        cout << "--------------------------------------------------------";
        cout << endl;
    }
    if (verbosity >= Verbosity::Debug)
    {
        vector<pair<double, double>> scanRes;
        MnPlot plot;
        MnScan scanner(minuitF, preMin.UserParameters(), 2);
        
        cout << "-- MINUIT scan -----------------------------------------";
        cout << endl;
        for (unsigned int i = 0; i < x.size(); i++)
        {
            cout << "Parameter x_" << i << endl;
            scanRes = scanner.Scan(i);
            plot(scanRes);
        }
        cout << "--------------------------------------------------------";
        cout << endl;
    }
    
    return x;
}
