/*
 * RootFinder.hpp, part of LatAnalyze 3
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

#ifndef Latan_RootFinder_hpp_
#define Latan_RootFinder_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Solver.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                              RootFinder                                    *
 ******************************************************************************/

class RootFinder: public Solver
{
public:
    // constructor
    explicit RootFinder(const Index dim);
    // destructor
    virtual ~RootFinder(void) = default;
    // solver
    virtual const DVec & operator()(const std::vector<DoubleFunction *> &func)
        = 0;
};

END_LATAN_NAMESPACE

#endif // Latan_RootFinder_hpp_
