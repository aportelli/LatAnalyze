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

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Mat.hpp>
#include <iostream>

#define FOR_STAT_ARRAY(ar, i) \
for (Latan::Index i = -(ar).offset; i < (ar).size(); ++i)

BEGIN_NAMESPACE

/******************************************************************************
 *                     Array class with statistics                            *
 ******************************************************************************/
template <typename T, Index os = 0>
class StatArray: public Array<T, dynamic, 1>
{
protected:
    typedef Array<T, dynamic, 1> Base;
public:
    // constructors
    StatArray(void);
    explicit StatArray(const Index size);
    EIGEN_EXPR_CTOR(StatArray, unique_arg(StatArray<T, os>), Base, ArrayExpr)
    // destructor
    virtual ~StatArray(void) = default;
    // access
    Index size(void) const;
    void  resize(const Index size);
    // operators
          T & operator[](const Index s);
    const T & operator[](const Index s) const;
    // statistics
    void bin(Index binSize);
    T    mean(const Index pos = 0, const Index n = -1) const;
    T    mean(void) const;
    T    covariance(const StatArray<T, os> &array, const Index pos = 0,
                    const Index n = -1) const;
    T    covarianceMatrix(const StatArray<T, os> &array, const Index pos = 0,
                          const Index n = -1) const;
    T    variance(const Index pos = 0, const Index n = -1) const;
    T    varianceMatrix(const Index pos = 0, const Index n = -1) const;
public:
    static const Index offset = os;
};

// reduction operations
namespace ReducOp
{
    // general templates
    template <typename T>
    inline T prod(const T &a, const T &b);
    template <typename T>
    inline T tensProd(const T &v1, const T &v2);
    template <typename T>
    inline T sum(const T &a, const T &b);
    // matrix specializations
}

// Sample types
#define SAMPLE_OFFSET 1

const int central = -SAMPLE_OFFSET;

template <typename T>
using Sample = StatArray<T, SAMPLE_OFFSET>;

typedef Sample<double>               DSample;
typedef Sample<std::complex<double>> CSample;

/******************************************************************************
 *                 StatArray class template implementation                    *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <typename T, Index os>
StatArray<T, os>::StatArray(void)
: Base(static_cast<typename Base::Index>(os))
{}

template <typename T, Index os>
StatArray<T, os>::StatArray(const Index size)
: Base(static_cast<typename Base::Index>(size + os))
{}

// access //////////////////////////////////////////////////////////////////////
template <typename T, Index os>
Index StatArray<T, os>::size(void) const
{
    return Base::size() - os;
}

template <typename T, Index os>
void StatArray<T, os>::resize(const Index size)
{
    Base::resize(size + os);
}

// operators ///////////////////////////////////////////////////////////////////
template <typename T, Index os>
T & StatArray<T, os>::operator[](const Index s)
{
    return Base::operator[](s + os);
}

template <typename T, Index os>
const T & StatArray<T, os>::operator[](const Index s) const
{
    return Base::operator[](s + os);
}


// statistics //////////////////////////////////////////////////////////////////
template <typename T, Index os>
void StatArray<T, os>::bin(Index binSize)
{
    Index q = size()/binSize, r = size()%binSize;

    for (Index i = 0; i < q; ++i)
    {
        (*this)[i] = mean(i*binSize, binSize);
    }
    if (r != 0)
    {
        (*this)[q] = mean(q*binSize, r);
        this->conservativeResize(os + q + 1);
    }
    else
    {
        this->conservativeResize(os + q);
    }
}

template <typename T, Index os>
T StatArray<T, os>::mean(const Index pos, const Index n) const
{
    T           result = T();
    const Index m = (n >= 0) ? n : size();
    
    if (m)
    {
        result = this->segment(pos+os, m).redux(&ReducOp::sum<T>);
    }
    
    return result/static_cast<double>(n);
}

template <typename T, Index os>
T StatArray<T, os>::covariance(const StatArray<T, os> &array, const Index pos,
                               const Index n) const
{
    T           s1, s2, prs, res = T();
    const Index m = (n >= 0) ? n : size();
    
    if (m)
    {
        auto arraySeg = array.segment(pos+os, m);
        auto thisSeg  = this->segment(pos+os, m);
        
        s1  = thisSeg.redux(&ReducOp::sum<T>);
        s2  = arraySeg.redux(&ReducOp::sum<T>);
        prs = thisSeg.binaryExpr(arraySeg, &ReducOp::prod<T>)
                     .redux(&ReducOp::sum<T>);
        res = prs - ReducOp::prod(s1, s2)/static_cast<double>(m);
    }
    
    return res/static_cast<double>(m - 1);
}

template <typename T, Index os>
T StatArray<T, os>::covarianceMatrix(const StatArray<T, os> &array,
                                     const Index pos, const Index n) const
{
    T           s1, s2, prs, res = T();
    const Index m = (n >= 0) ? n : size();
    
    if (m)
    {
        auto arraySeg = array.segment(pos+os, m);
        auto thisSeg  = this->segment(pos+os, m);

        s1  = thisSeg.redux(&ReducOp::sum<T>);
        s2  = arraySeg.redux(&ReducOp::sum<T>);
        prs = thisSeg.binaryExpr(arraySeg, &ReducOp::tensProd<T>)
                     .redux(&ReducOp::sum<T>);
        res = prs - ReducOp::tensProd(s1, s2)/static_cast<double>(m);
    }
    
    return res/static_cast<double>(m - 1);
}

template <typename T, Index os>
T StatArray<T, os>::variance(const Index pos, const Index n) const
{
    return covariance(*this, pos, n);
}

template <typename T, Index os>
T StatArray<T, os>::varianceMatrix(const Index pos, const Index n) const
{
    return covarianceMatrix(*this, pos, n);
}

// reduction operations ////////////////////////////////////////////////////////
template <typename T>
inline T ReducOp::sum(const T &a, const T &b)
{
    return a + b;
}

template <typename T>
inline T ReducOp::prod(const T &a, const T &b)
{
    return a*b;
}

template <typename T>
inline T ReducOp::tensProd(const T &v1 __unused, const T &v2 __unused)
{
    LATAN_ERROR(Implementation, 
                "tensorial product not implemented for this type");
}

template <>
inline Mat<double> ReducOp::prod(const Mat<double>  &a, const Mat<double>  &b)
{
    return a.cwiseProduct(b);
}

template <>
inline Mat<double> ReducOp::tensProd(const Mat<double>  &v1,
                                     const Mat<double>  &v2)
{
    if ((v1.cols() != 1)||(v2.cols() != 1))
    {
        LATAN_ERROR(Size,
                    "tensorial product is only valid with column vectors");
    }
    
    return v1*v2.transpose();
}

END_NAMESPACE

#endif // Latan_StatArray_hpp_
