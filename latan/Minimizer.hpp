/*
 * Minimizer.hpp, part of LatAnalyze 3
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

#ifndef Latan_Minimizer_hpp_
#define Latan_Minimizer_hpp_

#include <latan/Global.hpp>
#include <latan/Function.hpp>
#include <latan/Mat.hpp>

BEGIN_NAMESPACE

/******************************************************************************
 *                        Abstract minimizer class                            *
 ******************************************************************************/

class Minimizer
{
public:
    enum class Verbosity
    {
        Silent = 0,
        Normal = 1,
        Debug  = 2
    };
public:
    // constructor
    explicit Minimizer(Verbosity verbosity = Verbosity::Silent);
    // destructor
    virtual ~Minimizer(void) = default;
    // access
    Index        getDim(void) const;
    Verbosity    getVerbosity(void) const;
    void         setInit(const DVec &x0);
    void         setVerbosity(const Verbosity verbosity);
    // minimization
    virtual const DVec & operator()(const DoubleFunction &f) = 0;
protected:
    // access
    DVec & getState(void);
private:
    DVec      x_;
    Verbosity verbosity_;
};

END_NAMESPACE

#endif // Latan_Minimizer_hpp_
