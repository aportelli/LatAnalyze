/*
 * Function.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#ifndef Latan_Function_hpp_
#define	Latan_Function_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Core/Mat.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                            Double function class                           *
 ******************************************************************************/
class DoubleFunction
{
private:
    // function type
    typedef std::function<double(const double *)> vecFunc;
public:
    // constructor
    explicit DoubleFunction(const vecFunc &f = nullptr, const Index nArg = 0);
    // destructor
    virtual ~DoubleFunction(void) = default;
    // access
    virtual Index     getNArg(void) const;
            void      setFunction(const vecFunc &f, const Index nArg);
            VarName & varName(void);
    const   VarName & varName(void) const;
    // function call
    double operator()(const double *arg) const;
    double operator()(const DVec &arg) const;
    double operator()(const std::vector<double> &arg) const;
    double operator()(std::stack<double> &arg) const;
    double operator()(void) const;
    template <typename... Ts>
    double operator()(const double arg0, const Ts... args) const;
    std::map<double, double> operator()(const std::map<double, double> &m) const;
    // bind
    DoubleFunction bind(const Index argIndex, const double val) const;
    DoubleFunction bind(const Index argIndex, const DVec &x) const;
    // sample
    DVec sample(const DMat &x) const;
    // arithmetic operators
    DoubleFunction   operator-(void) const;
    DoubleFunction & operator+=(const DoubleFunction &f);
    DoubleFunction & operator+=(const DoubleFunction &&f);
    DoubleFunction & operator-=(const DoubleFunction &f);
    DoubleFunction & operator-=(const DoubleFunction &&f);
    DoubleFunction & operator*=(const DoubleFunction &f);
    DoubleFunction & operator*=(const DoubleFunction &&f);
    DoubleFunction & operator/=(const DoubleFunction &f);
    DoubleFunction & operator/=(const DoubleFunction &&f);
    DoubleFunction & operator+=(const double x);
    DoubleFunction & operator-=(const double x);
    DoubleFunction & operator*=(const double x);
    DoubleFunction & operator/=(const double x);
private:
    // error checking
    void checkSize(const Index nPar) const;
private:
    std::shared_ptr<DVec> buffer_{nullptr};
    VarName               varName_;
    vecFunc               f_;
};

/******************************************************************************
 *                  DoubleFunction template implementation                    *
 ******************************************************************************/
template <typename... Ts>
double DoubleFunction::operator()(const double arg0, const Ts... args) const
{
    static_assert(static_or<std::is_convertible<double, Ts>::value...>::value,
                  "DoubleFunction arguments are not compatible with double");

    const double arg[] = {arg0, static_cast<double>(args)...};
    
    checkSize(sizeof...(args) + 1);
    
    return (*this)(arg);
}

/******************************************************************************
 *                DoubleFunction inline arithmetic operators                  *
 ******************************************************************************/
#define MAKE_INLINE_FUNC_OP(op)\
inline DoubleFunction operator op(DoubleFunction lhs,\
                                  const DoubleFunction &rhs)\
{\
    lhs op##= rhs;\
    return lhs;\
}\
inline DoubleFunction operator op(DoubleFunction lhs,\
                                  const DoubleFunction &&rhs)\
{\
    return lhs op rhs;\
}

#define MAKE_INLINE_RSCALAR_OP(op)\
inline DoubleFunction operator op(DoubleFunction lhs, const double rhs)\
{\
    lhs op##= rhs;\
    return lhs;\
}\

#define MAKE_INLINE_LSCALAR_OP(op)\
inline DoubleFunction operator op(const double lhs, DoubleFunction rhs)\
{\
    rhs op##= lhs;\
    return rhs;\
}

MAKE_INLINE_FUNC_OP(+)
MAKE_INLINE_FUNC_OP(-)
MAKE_INLINE_FUNC_OP(*)
MAKE_INLINE_FUNC_OP(/)
MAKE_INLINE_RSCALAR_OP(+)
MAKE_INLINE_RSCALAR_OP(-)
MAKE_INLINE_RSCALAR_OP(*)
MAKE_INLINE_RSCALAR_OP(/)
MAKE_INLINE_LSCALAR_OP(+)
MAKE_INLINE_LSCALAR_OP(*)

// special case for scalar - function
inline DoubleFunction operator-(const double lhs, DoubleFunction rhs)
{
    return (-rhs) + lhs;
}

// special case for scalar/function
inline DoubleFunction operator/(const double lhs, DoubleFunction rhs)
{
    auto res = [lhs, rhs](const double *arg){return lhs/rhs(arg);};
    
    rhs.setFunction(res, rhs.getNArg());
    
    return rhs;
}

/******************************************************************************
 *                      DoubleFunctionSample class                            *
 ******************************************************************************/
class DoubleFunctionSample: public Sample<DoubleFunction>
{
public:
    // constructors
    DoubleFunctionSample(void);
    DoubleFunctionSample(const Index nSample);
    EIGEN_EXPR_CTOR(DoubleFunctionSample, DoubleFunctionSample,
                    Sample<DoubleFunction>, ArrayExpr)
    // destructor
    virtual ~DoubleFunctionSample(void) = default;
    // function call
    DSample operator()(const DMatSample &arg) const;
    DSample operator()(const double *arg) const;
    DSample operator()(const DVec &arg) const;
    DSample operator()(const std::vector<double> &arg) const;
    template <typename... Ts>
    DSample operator()(const double arg0, const Ts... args) const;
    // bind
    DoubleFunctionSample bind(const Index argIndex, const double val) const;
    DoubleFunctionSample bind(const Index argIndex, const DVec &x) const ;
};

template <typename... Ts>
DSample DoubleFunctionSample::operator()(const double arg0,
                                         const Ts... args) const
{
    const double arg[] = {arg0, static_cast<double>(args)...};
    
    return (*this)(arg);
}

/******************************************************************************
 *                       DoubleFunctionFactory class                          *
 ******************************************************************************/
class DoubleFunctionFactory
{
public:
    // constructor
    DoubleFunctionFactory(void) = default;
    // destructor
    virtual ~DoubleFunctionFactory(void) = default;
    // factory
    virtual DoubleFunction makeFunction(const bool makeHardCopy) const = 0;
};

END_LATAN_NAMESPACE

#endif // Latan_Function_hpp_
