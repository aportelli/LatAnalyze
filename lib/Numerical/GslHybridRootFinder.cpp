/*
 * GslHybridRootFinder.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Numerical/GslHybridRootFinder.hpp>
#include <LatAnalyze/includes.hpp>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                 GslHybridRootFinder implementation                          *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
GslHybridRootFinder::GslHybridRootFinder(const Index dim)
: RootFinder(dim)
{}

// output //////////////////////////////////////////////////////////////////////
void GslHybridRootFinder::printState(void)
{
    if (solver_)
    {
        cout << "x=";
        for (size_t i = 0; i < solver_->x->size; ++i)
        {
            cout << " " << scientific << gsl_vector_get(solver_->x, i);
        }
        cout << endl;
        cout << "f=";
        for (size_t i = 0; i < solver_->f->size; ++i)
        {
            cout << " " << scientific << gsl_vector_get(solver_->f, i);
        }
        cout << endl;
    }
}

// solver //////////////////////////////////////////////////////////////////////
const DVec &
GslHybridRootFinder::operator()(const vector<DoubleFunction *> &func)
{
    DVec                   &res      = getState();
    Verbosity              verbosity = getVerbosity();
    int                    status;
    unsigned int           iter      = 0;
    const size_t           nFunc     = func.size();
    Index                  nArg;
    gsl_vector             *x;
    gsl_multiroot_function fStruct;
    
    int (*fWrap)(const gsl_vector *, void *, gsl_vector *) =
        [](const gsl_vector *var, void *vFunc, gsl_vector *f)->int
    {
        vector<DoubleFunction *> &fPt =
            *static_cast<vector<DoubleFunction *> *>(vFunc);
        
        for (unsigned int i = 0; i < fPt.size(); ++i)
        {
            gsl_vector_set(f, i, (*fPt[i])(var->data));
        }
        
        return GSL_SUCCESS;
    };
    
    nArg = func[0]->getNArg();
    for (auto f: func)
    {
        if (f->getNArg() != nArg)
        {
            LATAN_ERROR(Size,
                        "equations do not have the same number of unknown");
        }
    }
    if (nArg != static_cast<Index>(nFunc))
    {
        LATAN_ERROR(Size, "equation and unknown number mismatch");
    }
    if (res.size() != nArg)
    {
        resize(nArg);
    }
    solver_ = gsl_multiroot_fsolver_alloc(gsl_multiroot_fsolver_hybrids, nFunc);
    x       = gsl_vector_alloc(nFunc);
    FOR_VEC(res, i)
    {
        gsl_vector_set(x, static_cast<size_t>(i), res(i));
    }
    fStruct.n      = nFunc;
    fStruct.params = reinterpret_cast<void *>(
                        const_cast<vector<DoubleFunction *> *>(&func));
    fStruct.f      = fWrap;
    gsl_multiroot_fsolver_set(solver_, &fStruct, x);
    do
    {
        iter++;
        status = gsl_multiroot_fsolver_iterate(solver_);
        if (verbosity >= Verbosity::Debug)
        {
            cout << "--- iteration " << iter << endl;
            printState();
        }
        if (status)
        {
            break;
        }
        status = gsl_multiroot_test_residual(solver_->f, getPrecision());
    } while ((status == GSL_CONTINUE) and (iter < getMaxIteration()));
    if (verbosity >= Verbosity::Debug)
    {
        cout << "--- done" << endl;
        cout << "end status: " << gsl_strerror(status) << endl;
    }
    if (status)
    {
        LATAN_WARNING("GSL hybrid root finder ended with status '" +
                      strFrom(gsl_strerror(status)) + "'");
    }
    FOR_VEC(res, i)
    {
        res(i) = gsl_vector_get(solver_->x, static_cast<size_t>(i));
    }
    gsl_vector_free(x);
    gsl_multiroot_fsolver_free(solver_);
    solver_ = nullptr;
    
    return res;
}
