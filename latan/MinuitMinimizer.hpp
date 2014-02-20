/*
 * MinuitMinimizer.hpp, part of LatAnalyze 3
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

#ifndef Latan_MinuitMinimizer_hpp_
#define Latan_MinuitMinimizer_hpp_

#include <latan/Global.hpp>
#include <latan/Function.hpp>
#include <latan/Minimizer.hpp>
#include <Minuit2/FCNBase.h>

BEGIN_NAMESPACE

/******************************************************************************
 *                             Minuit minimizer                               *
 ******************************************************************************/

class MinuitMinimizer: public Minimizer
{
private:
    class MinuitFunction: public ROOT::Minuit2::FCNBase
    {
    public:
        // constructor
        explicit MinuitFunction(const DoubleFunction &f);
        // destructor
        virtual ~MinuitFunction(void) = default;
        // minuit members
        virtual double operator()(const std::vector<double> &x) const;
        virtual double Up(void) const;
    private:
        const DoubleFunction *f_;
    };
public:
    // constructors
    using Minimizer::Minimizer;
    // destructor
    virtual ~MinuitMinimizer(void) = default;
    // minimization
    virtual const DVec & operator()(const DoubleFunction &f);
};

END_NAMESPACE

#endif // Latan_MinuitMinimizer_hpp_
