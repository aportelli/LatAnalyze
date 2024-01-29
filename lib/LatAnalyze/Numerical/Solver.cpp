/*
 * Solver.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Numerical/Solver.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                        Solver implementation                               *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
Solver::Solver(const double precision, const unsigned int maxIteration)
{
    setMaxIteration(maxIteration);
    setPrecision(precision);
}

Solver::Solver(const Index dim, const double precision,
               const unsigned int maxIteration)
: Solver(precision, maxIteration)
{
    resize(dim);
}

// access //////////////////////////////////////////////////////////////////////
Index Solver::getDim(void) const
{
    return x_.size();
}

unsigned int Solver::getMaxIteration(void) const
{
    return maxIteration_;
}

double Solver::getPrecision(void) const
{
    return precision_;
}

DVec & Solver::getState(void)
{
    return x_;
}

Solver::Verbosity Solver::getVerbosity(void) const
{
    return verbosity_;
}

void Solver::setInit(const DVec &x0)
{
    if (x0.size() != x_.size())
    {
        resize(x0.size());
    }
    x_ = x0;
}

void Solver::setMaxIteration(const unsigned int maxIteration)
{
    maxIteration_ = maxIteration;
}

void Solver::setPrecision(const double precision)
{
    precision_ = precision;
}

void Solver::setVerbosity(const Verbosity verbosity)
{
    verbosity_ = verbosity;
}

void Solver::resize(const Index dim)
{
    x_.resize(dim);
}
