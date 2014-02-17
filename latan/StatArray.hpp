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
#include <iostream>

BEGIN_NAMESPACE

/******************************************************************************
 *                     Array class with statistics                            *
 ******************************************************************************/
template <typename T, unsigned int offset = 0>
class StatArray: public Eigen::Array<T, Eigen::Dynamic, 1>
{
private:
    typedef Eigen::Array<T, Eigen::Dynamic, 1> Base;
public:
    // constructors
    StatArray(void);
    StatArray(const unsigned int size);
    EIGEN_EXPR_CTOR(StatArray, unique_arg(StatArray<T, offset>), Base,
                    ArrayBase);
    // destructor
    virtual ~StatArray(void);
    // access
    unsigned int size(void) const;
    // operators
          T & operator[](const int s);
    const T & operator[](const int s) const;
    // statistics
    void bin(unsigned int binSize);
    T    mean(const unsigned int pos, const unsigned int n) const;
    T    mean(void) const;
    T    variance(const unsigned int pos, const unsigned int n) const;
    T    variance(void) const;
};

// reduction operations
namespace ReducOp
{
    template <typename T>
    inline T square(const T &a);
    template <typename T>
    inline T sum(const T &a, const T &b);
    template <>
    inline DMat square(const DMat &a);
}

/******************************************************************************
 *                 StatArray class template implementation                    *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <typename T, unsigned int offset>
StatArray<T, offset>::StatArray(void)
: Base(static_cast<typename Base::Index>(offset))
{}

template <typename T, unsigned int offset>
StatArray<T, offset>::StatArray(const unsigned int size)
: Base(static_cast<typename Base::Index>(size + offset))
{}

// destructor //////////////////////////////////////////////////////////////////
template <typename T, unsigned int offset>
StatArray<T, offset>::~StatArray(void)
{}

// access //////////////////////////////////////////////////////////////////////
template <typename T, unsigned int offset>
unsigned int StatArray<T, offset>::size(void) const
{
    return Base::size() - offset;
}

// operators ///////////////////////////////////////////////////////////////////
template <typename T, unsigned int offset>
T & StatArray<T, offset>::operator[](const int s)
{
    return Base::operator[](s + offset);
}

template <typename T, unsigned int offset>
const T & StatArray<T, offset>::operator[](const int s) const
{
    return Base::operator[](s + offset);
}


// statistics //////////////////////////////////////////////////////////////////
template <typename T, unsigned int offset>
void StatArray<T, offset>::bin(unsigned int binSize)
{
    unsigned int q = size()/binSize, r = size()%binSize;

    for (unsigned int i = 0; i < q; ++i)
    {
        (*this)[i] = mean(i*binSize, binSize);
    }
    if (r != 0)
    {
        (*this)[q] = mean(q*binSize, r);
        this->conservativeResize(offset + q + 1);
    }
    else
    {
        this->conservativeResize(offset + q);
    }
}

template <typename T, unsigned int offset>
T StatArray<T, offset>::mean(const unsigned int pos, const unsigned int n) const
{
    T result;
    
    if (n)
    {
        result = this->segment(pos+offset, n).redux(&ReducOp::sum<T>);
    }
    
    return result/static_cast<double>(n);
}

template <typename T, unsigned int offset>
T StatArray<T, offset>::mean(void) const
{
    return mean(0, size());
}

template <typename T, unsigned int offset>
T StatArray<T, offset>::variance(const unsigned int pos, const unsigned int n) const
{
    T s, sqs, result;
    
    if (n)
    {
        s   = this->segment(pos+offset, n).redux(&ReducOp::sum<T>);
        sqs = this->segment(pos+offset, n).unaryExpr(&ReducOp::square<T>)
                                          .redux(&ReducOp::sum<T>);
        result = sqs - ReducOp::square(s)/static_cast<double>(n);
    }
    
    return result/static_cast<double>(n - 1);
}

template <typename T, unsigned int offset>
T StatArray<T, offset>::variance(void) const
{
    return variance(0, size());
}

// reduction operations ////////////////////////////////////////////////////////
template <typename T>
inline T ReducOp::sum(const T &a, const T &b)
{
    return a + b;
}

template <typename T>
inline T ReducOp::square(const T &a)
{
    return a*a;
}

template <>
inline DMat ReducOp::square(const DMat &a)
{
    return a.cwiseProduct(a);
}

END_NAMESPACE

#endif // Latan_StatArray_hpp_
