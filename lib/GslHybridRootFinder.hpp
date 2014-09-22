/*
 * GslHybridRootFinder.hpp, part of LatAnalyze 3
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

#ifndef Latan_GslHybridRootFinder_hpp_
#define Latan_GslHybridRootFinder_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/RootFinder.hpp>
#include <gsl/gsl_multiroots.h>

BEGIN_NAMESPACE

/******************************************************************************
 *                         GslHybridRootFinder                                 *
 ******************************************************************************/

class GslHybridRootFinder: public RootFinder
{
public:
    // constructor
    GslHybridRootFinder(void) = default;
    // destructor
    virtual ~GslHybridRootFinder(void) = default;
    // solver
    virtual const DVec & operator()(const std::vector<DoubleFunction *> &func);
private:
    // output
    void printState(void);
private:
    gsl_multiroot_fsolver *solver_{nullptr};
};

END_NAMESPACE

#endif // Latan_GslHybridRootFinder_hpp_
