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

using namespace std;
using namespace ROOT;
using namespace Minuit2;
using namespace Latan;

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

// minimization ////////////////////////////////////////////////////////////////
const DVec & MinuitMinimizer::operator()(const DoubleFunction &f)
{
    DVec &x = getState();
    Verbosity verbosity = getVerbosity();
    
    if (f.getNArg() != x.size())
    {
        x.conservativeResize(f.getNArg());
    }
    
    // set parameters
    MinuitFunction   minuitF(f);
    MnUserParameters parameters;
    
    for (Index i = 0; i < x.size(); ++i)
    {
        parameters.Add("x_" + strFrom(i), x(i), 0.1*fabs(x(i)));
    }
    
    // pre-minimization
    MnMigrad        migrad1(minuitF, parameters, 1);
    FunctionMinimum min = migrad1();
    
    if (verbosity >= Verbosity::Debug)
    {
        cout << "-- MINUIT pre-minimization -----------------------------";
        cout << min;
        cout << "--------------------------------------------------------";
        cout << endl;
    }
    for (unsigned int i = 0; i < x.size(); ++i)
    {
        parameters.SetValue(i, min.UserParameters().Value(i));
        parameters.SetError(i, min.UserParameters().Error(i));
    }
    
    // minimization and output
    MnMigrad migrad2(minuitF, parameters, 2);
    
    min = migrad2();
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
        MnScan scanner(minuitF, min.UserParameters(), 2);
        
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
