/*
 * Minimizer.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Minimizer.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                       Minimizer implementation                             *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
Minimizer::Minimizer(Verbosity verbosity)
: verbosity_(verbosity)
{}

// access //////////////////////////////////////////////////////////////////////
Index Minimizer::getDim(void) const
{
    return x_.size();
}

DVec & Minimizer::getState(void)
{
    return x_;
}

Minimizer::Verbosity Minimizer::getVerbosity(void) const
{
    return verbosity_;
}

void Minimizer::setInit(const DVec &x0)
{
    x_ = x0;
}

void Minimizer::setVerbosity(const Verbosity verbosity)
{
    verbosity_ = verbosity;
}
