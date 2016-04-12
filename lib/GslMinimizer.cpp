/*
 * GslMinimizer.cpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <LatAnalyze/GslMinimizer.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Math.hpp>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_blas.h>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                    GslMinimizer implementation                             *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
GslMinimizer::GslMinimizer(const Algorithm algorithm)
{
    setAlgorithm(algorithm);
    der_.setOrder(1, 1);
}

// access //////////////////////////////////////////////////////////////////////
GslMinimizer::Algorithm GslMinimizer::getAlgorithm(void) const
{
    return algorithm_;
}

void GslMinimizer::setAlgorithm(const Algorithm algorithm)
{
    algorithm_ = algorithm;
}

bool GslMinimizer::supportLimits(void) const
{
    return false;
}

// test ////////////////////////////////////////////////////////////////////////
bool GslMinimizer::isDerAlgorithm(const Algorithm algorithm)
{
    return (algorithm <= Algorithm::lastDerAlg);
}

// minimization ////////////////////////////////////////////////////////////////
const DVec & GslMinimizer::operator()(const DoubleFunction &f)
{
    DVec &x = getState();
    
    // resize minimizer state to match function number of arguments
    if (f.getNArg() != x.size())
    {
        resize(f.getNArg());
    }
    
    // set function data
    GslFuncData data;
    
    der_.setFunction(f);
    data.f = &f;
    data.d = &der_;
    
    // set initial position
    gsl_vector *gslX = gsl_vector_alloc(getDim());
    
    for (Index i = 0; i < getDim(); ++i)
    {
        gsl_vector_set(gslX, i, x(i));
    }
    
    // minimization
    int status;
    
    if (isDerAlgorithm(getAlgorithm()))
    {
        // set function
        gsl_multimin_function_fdf gslFunc;
        
        gslFunc.n      = getDim();
        gslFunc.f      = &fWrapper;
        gslFunc.df     = &dfWrapper;
        gslFunc.fdf    = &fdfWrapper;
        gslFunc.params = &data;
        
        // create and set minimizer
        const gsl_multimin_fdfminimizer_type *gslAlg;
        gsl_multimin_fdfminimizer            *gslMin;
        
        switch (getAlgorithm())
        {
            case Algorithm::cgFR:
                gslAlg = gsl_multimin_fdfminimizer_conjugate_fr;
                break;
            case Algorithm::cgPR:
                gslAlg = gsl_multimin_fdfminimizer_conjugate_pr;
                break;
            case Algorithm::bfgs:
                gslAlg = gsl_multimin_fdfminimizer_vector_bfgs;
                break;
            case Algorithm::bfgs2:
                gslAlg = gsl_multimin_fdfminimizer_vector_bfgs2;
                break;
            case Algorithm::steepDesc:
                gslAlg = gsl_multimin_fdfminimizer_vector_bfgs2;
                break;
            default:
                LATAN_ERROR(Argument, "unknow GSL minization algorithm "
                            + strFrom(static_cast<int>(getAlgorithm())));
                break;
        }
        gslMin = gsl_multimin_fdfminimizer_alloc(gslAlg, getDim());
        
        // minimize
        unsigned int pass = 0, it;
        double       dxRel;
        
        do
        {
            pass++;
            gsl_multimin_fdfminimizer_set(gslMin, &gslFunc, gslX, 0.01, 0.001);
            if (getVerbosity() >= Verbosity::Normal)
            {
                cout << "========== GSL minimization, pass #" << pass;
                cout << " ==========" << endl;
                cout << "Algorithm: " << getAlgorithmName(getAlgorithm());
                cout << endl;
                cout << "Max eval.= " << getMaxIteration();
                cout << " -- Precision= " << getPrecision() << endl;
                printf("Starting f(x)= %.10e\n", f(x));
            }
            it = 0;
            do
            {
                it++;
                gsl_multimin_fdfminimizer_iterate(gslMin);
                dxRel  = gsl_blas_dnrm2(gslMin->dx)/gsl_blas_dnrm2(gslMin->x);
                status = (dxRel < getPrecision()) ? GSL_SUCCESS : GSL_CONTINUE;
                if (getVerbosity() >= Verbosity::Debug)
                {
                    printf("iteration %4d: f= %.10e dxrel= %.10e eval= %d\n",
                           it, gslMin->f, dxRel, data.evalCount);
                }
                
            } while (status == GSL_CONTINUE and
                     (data.evalCount < getMaxIteration()));
            if (getVerbosity() >= Verbosity::Normal)
            {
                printf("Found minimum %.10e at:\n", gslMin->f);
                for (Index i = 0; i < x.size(); ++i)
                {
                    printf("%8s= %.10e\n", f.varName().getName(i).c_str(),
                           gsl_vector_get(gslMin->x, i));
                }
                cout << "after " << data.evalCount << " evaluations" << endl;
                cout << "Minimization ended with code " << status;
                cout << endl;
            }
            data.evalCount = 0;
            for (Index i = 0; i < getDim(); ++i)
            {
                gsl_vector_set(gslX, i, gsl_vector_get(gslMin->x, i));
            }
        } while (status != GSL_SUCCESS and (pass < getMaxPass()));
        
        // deallocate GSL minimizer
        gsl_multimin_fdfminimizer_free(gslMin);
    }
    else
    {
        // set function
        gsl_multimin_function gslFunc;
        
        gslFunc.n      = getDim();
        gslFunc.f      = &fWrapper;
        gslFunc.params = &data;
        
        // create and set minimizer
        const gsl_multimin_fminimizer_type *gslAlg;
        gsl_multimin_fminimizer            *gslMin;
        
        switch (getAlgorithm())
        {
            case Algorithm::simplex:
                gslAlg = gsl_multimin_fminimizer_nmsimplex;
                break;
            case Algorithm::simplex2:
                gslAlg = gsl_multimin_fminimizer_nmsimplex2;
                break;
            case Algorithm::simplex2R:
                gslAlg = gsl_multimin_fminimizer_nmsimplex2rand;
                break;
            default:
                LATAN_ERROR(Argument, "unknow GSL minization algorithm "
                            + strFrom(static_cast<int>(getAlgorithm())));
                break;
        }
        gslMin = gsl_multimin_fminimizer_alloc(gslAlg, getDim());
        
        // minimize
        unsigned int pass = 0, it;
        gsl_vector   *step = gsl_vector_alloc(getDim());
        double       relSize;
        
        gsl_vector_set_all(step, 0.01);
        do
        {
            pass++;
            gsl_multimin_fminimizer_set(gslMin, &gslFunc, gslX, step);
            if (getVerbosity() >= Verbosity::Normal)
            {
                cout << "========== GSL minimization, pass #" << pass;
                cout << " ==========" << endl;
                cout << "Algorithm: " << getAlgorithmName(getAlgorithm());
                cout << endl;
                cout << "Max eval.= " << getMaxIteration();
                cout << " -- Precision= " << getPrecision() << endl;
                printf("Starting f(x)= %.10e\n", f(x));
            }
            it = 0;
            do
            {
                it++;
                gsl_multimin_fminimizer_iterate(gslMin);
                relSize = Math::pow<2>(gslMin->size)/gsl_blas_dnrm2(gslMin->x);
                status  = (relSize < getPrecision()) ? GSL_SUCCESS
                                                     : GSL_CONTINUE;
                if (getVerbosity() >= Verbosity::Debug)
                {
                    printf("iteration %4d: f= %.10e relSize= %.10e eval= %d\n",
                           it, gslMin->fval, relSize, data.evalCount);
                }
                
            } while (status == GSL_CONTINUE and
                     (data.evalCount < getMaxIteration()));
            if (getVerbosity() >= Verbosity::Normal)
            {
                printf("Found minimum %.10e at:\n", gslMin->fval);
                for (Index i = 0; i < x.size(); ++i)
                {
                    printf("%8s= %.10e\n", f.varName().getName(i).c_str(),
                           gsl_vector_get(gslMin->x, i));
                }
                cout << "after " << data.evalCount << " evaluations" << endl;
                cout << "Minimization ended with code " << status;
                cout << endl;
            }
            data.evalCount = 0;
            for (Index i = 0; i < getDim(); ++i)
            {
                gsl_vector_set(gslX, i, gsl_vector_get(gslMin->x, i));
            }
        } while (status != GSL_SUCCESS and (pass < getMaxPass()));
        
        // deallocate GSL minimizer
        gsl_multimin_fminimizer_free(gslMin);
        gsl_vector_free(step);
    }
    if (status != GSL_SUCCESS)
    {
        LATAN_WARNING("invalid minimum: maximum number of call reached");
    }
    
    // save final result
    for (Index i = 0; i < getDim(); ++i)
    {
        x(i) = gsl_vector_get(gslX, i);
    }
    
    // deallocate GSL state and return
    gsl_vector_free(gslX);
    
    return x;
}

// function wrappers ///////////////////////////////////////////////////////////
double GslMinimizer::fWrapper(const gsl_vector *x, void *vdata)
{
    GslFuncData &data = *static_cast<GslFuncData *>(vdata);
    
    data.evalCount++;
    
    return (*data.f)(x->data);
}

void GslMinimizer::dfWrapper(const gsl_vector *x, void *vdata, gsl_vector * df)
{
    GslFuncData        &data = *static_cast<GslFuncData *>(vdata);
    const unsigned int n     = data.f->getNArg();
    
    for (unsigned int i = 0; i < n; ++i)
    {
        data.d->setDir(i);
        gsl_vector_set(df, i, (*(data.d))(x->data));
    }
    data.evalCount += data.d->getNPoint()*n;
}

void GslMinimizer::fdfWrapper(const gsl_vector *x, void *vdata, double *f,
                              gsl_vector * df)
{
    GslFuncData        &data = *static_cast<GslFuncData *>(vdata);
    const unsigned int n     = data.f->getNArg();
    
    for (unsigned int i = 0; i < n; ++i)
    {
        data.d->setDir(i);
        gsl_vector_set(df, i, (*(data.d))(x->data));
    }
    *f = (*data.f)(x->data);
    data.evalCount += data.d->getNPoint()*n + 1;
}

// algorithm names /////////////////////////////////////////////////////////////
string GslMinimizer::getAlgorithmName(const Algorithm algorithm)
{
    switch (algorithm)
    {
        case Algorithm::cgFR:
            return "Fletcher-Reeves conjugate gradient";
            break;
        case Algorithm::cgPR:
            return "Polak-Ribiere conjugate gradient";
            break;
        case Algorithm::bfgs:
            return "Broyden-Fletcher-Goldfarb-Shanno";
            break;
        case Algorithm::bfgs2:
            return "improved Broyden-Fletcher-Goldfarb-Shanno";
            break;
        case Algorithm::steepDesc:
            return "steepest descent";
            break;
        case Algorithm::simplex:
            return "Nelder-Mead simplex";
            break;
        case Algorithm::simplex2:
            return "improved Nelder-Mead simplex";
            break;
        case Algorithm::simplex2R:
            return "improved Nelder-Mead simplex with random start";
            break;
    }
}
