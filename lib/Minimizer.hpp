/*
 * Minimizer.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli
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

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Mat.hpp>
#include <LatAnalyze/Solver.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                        Abstract minimizer class                            *
 ******************************************************************************/

class Minimizer: public Solver
{
public:
    // constructor
    Minimizer(void) = default;
    explicit Minimizer(const Index dim);
    // destructor
    virtual ~Minimizer(void) = default;
    // access
    virtual void resize(const Index dim);
    // limits
    virtual double       getHighLimit(const Index i) const ;
    virtual const DVec & getHighLimit(const PlaceHolder ph = _) const;
    virtual double       getLowLimit(const Index i) const;
    virtual const DVec & getLowLimit(const PlaceHolder ph = _) const;
    virtual bool         hasHighLimit(const Index i) const;
    virtual bool         hasLowLimit(const Index i) const;
    virtual void         setHighLimit(const Index i, const double l);
    virtual void         setHighLimit(const PlaceHolder ph, const DVec &l);
    virtual void         setLowLimit(const Index i, const double l);
    virtual void         setLowLimit(const PlaceHolder ph, const DVec &l);
    virtual void         useHighLimit(const Index i, const bool use = true);
    virtual void         useHighLimit(const PlaceHolder ph = _,
                                      const bool use = true);
    virtual void         useLowLimit(const Index i, const bool use = true);
    virtual void         useLowLimit(const PlaceHolder ph = _,
                                     const bool use = true);
    // minimization
    virtual const DVec & operator()(const DoubleFunction &f) = 0;
private:
    DVec      highLimit_, lowLimit_;
    Vec<bool> hasHighLimit_, hasLowLimit_;
};

END_LATAN_NAMESPACE

#endif // Latan_Minimizer_hpp_
