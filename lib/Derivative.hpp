/*
 * Derivative.hpp, part of LatAnalyze 3
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

#ifndef Latan_Derivative_hpp_
#define Latan_Derivative_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                              Derivative                                    *
 ******************************************************************************/
class Derivative: public DoubleFunctionFactory
{
public:
    static constexpr double defaultStep = 1.0e-2;
public:
    // constructor
    Derivative(const DoubleFunction &f, const Index dir, const Index order,
               const DVec &point, const double step = defaultStep);
    // destructor
    virtual ~Derivative(void) = default;
    // access
    Index  getDir(void) const;
    Index  getNPoint(void) const;
    Index  getOrder(void) const;
    double getStep(void) const;
    void   setDir(const Index dir);
    void   setFunction(const DoubleFunction &f);
    void   setOrderAndPoint(const Index order, const DVec &point);
    void   setStep(const double step);
    // function call
    double operator()(const double *x) const;
    // function factory
    virtual DoubleFunction makeFunction(const bool makeHardCopy = true) const;
protected:
    // constructor
    Derivative(const DoubleFunction &f, const Index dir,
               const double step = defaultStep);
private:
    void makeCoefficients(void);
private:
    DoubleFunction        f_;
    Index                 dir_, order_;
    double                step_;
    DVec                  point_, coefficient_;
    std::shared_ptr<DVec> buffer_;
};

DoubleFunction derivative(const DoubleFunction &f, const Index dir,
                          const Index order, const DVec point,
                          const double step = Derivative::defaultStep);

class CentralDerivative: public Derivative
{
public:
    static const Index defaultPrecOrder = 2;
public:
    // constructor
    CentralDerivative(const DoubleFunction &f = DoubleFunction(),
                      const Index dir = 0,
                      const Index order = 1,
                      const Index precOrder = defaultPrecOrder);
    // destructor
    virtual ~CentralDerivative(void) = default;
    // access
    Index getPrecOrder(void) const;
    void  setOrder(const Index order, const Index precOrder = defaultPrecOrder);
    // function call
    using Derivative::operator();
private:
    // step tuning
    void tuneStep(void);
private:
    Index precOrder_;
};

DoubleFunction centralDerivative(const DoubleFunction &f, const Index dir = 0,
                                 const Index order = 1,
                                 const Index precOrder =
                                    CentralDerivative::defaultPrecOrder);

END_LATAN_NAMESPACE

#endif // Latan_Derivative_hpp_
