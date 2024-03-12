/*
 * CompositeMinimizer.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2024 Antonin Portelli
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

#include <LatAnalyze/Numerical/CompositeMinimizer.hpp>
#include <LatAnalyze/includes.hpp>
#include "CompositeMinimizer.hpp"

using namespace std;
using namespace Latan;

/******************************************************************************
 *                   CompositeMinimizer implementation                        *
 ******************************************************************************/
// constructor ////////////////////////////////////////////////////////////////
CompositeMinimizer::CompositeMinimizer(const std::vector<Minimizer *> &minimizer)
: minVec_(minimizer)
{
    if (minVec_.size() == 0)
    {
        LATAN_ERROR(Size, "minimizer vector has size 0");
    }
}

// access /////////////////////////////////////////////////////////////////////
bool CompositeMinimizer::supportLimits(void) const
{
    return false;
}

// minimization ///////////////////////////////////////////////////////////////
const DVec &CompositeMinimizer::operator()(const DoubleFunction &f)
{
    DVec &x = getState();
    DVec tmp;
    unsigned int i = 1, nMin = minVec_.size();

    // resize minimizer state to match function number of arguments
    if (f.getNArg() != x.size())
    {
        resize(f.getNArg());
    }

    for (auto &m: minVec_)
    {
        if (getVerbosity() >= Verbosity::Normal)
        {
            cout << "********** Composite minimizer step " << i << "/" << nMin << endl;
        }
        m->setInit(x);
        x = (*m)(f);
        i++;
    }

    return x;
}
