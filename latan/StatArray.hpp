/*
 * StatArray.hpp, part of LatAnalyze 3
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

#ifndef Latan_StatArray_hpp_
#define Latan_StatArray_hpp_

#include <latan/Global.hpp>
#include <latan/Mat.hpp>

BEGIN_NAMESPACE

/******************************************************************************
 *                     Array class with statistics                            *
 ******************************************************************************/
template <typename T>
class StatArray: public Eigen::Array<T, Eigen::Dynamic, 1>
{
private:
    typedef Eigen::Array<T, Eigen::Dynamic, 1> Base;
public:
    // constructors
    StatArray(void);
    StatArray(const unsigned int size);
    template <typename Derived>
    StatArray(const Eigen::EigenBase<Derived> &s);
    // destructor
    virtual ~StatArray(void);
    // statistics
    T mean(void)     const;
    T variance(void) const;
private:
    // index of the first element to take into account for statistics
    virtual unsigned int getOffset(void) const;
    // operations for reduction in statistical computations
    static inline T square(const T &a);
    static inline T sum(const T &a, const T &b);
};

template <>
inline DMat StatArray<DMat>::square(const DMat &a);

/******************************************************************************
 *                 StatArray class template implementation                    *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <typename T>
StatArray<T>::StatArray(void)
: Base(static_cast<typename Base::Index>(1))
{}

template <typename T>
StatArray<T>::StatArray(const unsigned int size)
: Base(static_cast<typename Base::Index>(size))
{}

template <typename T>
template <typename Derived>
StatArray<T>::StatArray(const  Eigen::EigenBase<Derived> &s)
: Base(s)
{}

// destructor //////////////////////////////////////////////////////////////////
template <typename T>
StatArray<T>::~StatArray(void)
{}

// statistics //////////////////////////////////////////////////////////////////
template <typename T>
T StatArray<T>::mean(void) const
{
    T result;
    unsigned int size = this->size() - getOffset();
    
    if (size)
    {
        result = this->tail(size).redux(&StatArray<T>::sum);
    }
    
    return result/static_cast<double>(size);
}

template <typename T>
T StatArray<T>::variance(void) const
{
    T s, sqs, result;
    unsigned int size = this->size() - getOffset();
    
    if (size)
    {
        s      = this->tail(size).redux(&StatArray<T>::sum);
        sqs    = this->tail(size).unaryExpr(&StatArray<T>::square)
                                 .redux(&StatArray<T>::sum);
        result = sqs - square(s)/static_cast<double>(size);
    }
    
    return result/static_cast<double>(size - 1);
}

template <typename T>
inline T StatArray<T>::sum(const T &a, const T &b)
{
    return a + b;
}

template <typename T>
inline T StatArray<T>::square(const T &a)
{
    return a*a;
}

template <>
inline DMat StatArray<DMat>::square(const DMat &a)
{
    return a.cwiseProduct(a);
}

template <typename T>
unsigned int StatArray<T>::getOffset(void) const
{
    return 0u;
}

END_NAMESPACE

#endif // Latan_StatArray_hpp_
