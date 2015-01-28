/*
 * Solver.hpp, part of LatAnalyze 3
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

#ifndef Latan_Solver_hpp_
#define Latan_Solver_hpp_

#include <LatAnalyze/Global.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                                Solver                                      *
 ******************************************************************************/

class Solver
{
public:
    static const     unsigned int defaultMaxIteration = 1000u;
    static constexpr double       defaultPrec = 1.0e-7;
public:
    enum class Verbosity
    {
        Silent = 0,
        Normal = 1,
        Debug  = 2
    };
public:
    // constructor
    explicit Solver(const Index dim, const double precision = defaultPrec,
                    const unsigned int maxIteration = defaultMaxIteration);
    // destructor
    virtual ~Solver(void) = default;
    // access
            Index        getDim(void) const;
    virtual double       getPrecision(void) const;
    virtual unsigned int getMaxIteration(void) const;
            Verbosity    getVerbosity(void) const;
    virtual void         setInit(const DVec &x0);
    virtual void         setPrecision(const double precision);
    virtual void         setMaxIteration(const unsigned int maxIteration);
            void         setVerbosity(const Verbosity verbosity);
protected:
    // access
    DVec & getState(void);
private:
    unsigned int maxIteration_;
    double       precision_;
    DVec         x_;
    Verbosity    verbosity_{Verbosity::Silent};
};

END_LATAN_NAMESPACE

#endif // Latan_Solver_hpp_
