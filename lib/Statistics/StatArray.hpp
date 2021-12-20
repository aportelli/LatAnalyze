/*
 * StatArray.hpp, part of LatAnalyze 3
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

#ifndef Latan_StatArray_hpp_
#define Latan_StatArray_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Core/Mat.hpp>

#define FOR_STAT_ARRAY(ar, i) \
for (Latan::Index i = -(ar).offset; i < (ar).size(); ++i)

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                     Array class with statistics                            *
 ******************************************************************************/
template <typename T, Index os = 0>
class StatArray: public Array<T, dynamic, 1>, public IoObject
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
    T    sum(const Index pos = 0, const Index n = -1) const;
    T    meanOld(const Index pos = 0, const Index n = -1) const;
    T    mean(const Index pos = 0, const Index n = -1) const;
    T    covarianceOld(const StatArray<T, os> &array) const;
    T    covariance(const StatArray<T, os> &array) const;
    T    covarianceMatrixOld(const StatArray<T, os> &array, const Index pos = 0,
                          const Index n = -1) const;
    T    varianceOld(void) const;
    T    variance(void) const;
    T    varianceMatrixOld(const Index pos = 0, const Index n = -1) const;
    T    correlationMatrixOld(const Index pos = 0, const Index n = -1) const;
    // IO type
    virtual IoType getType(void) const;
public:
    static constexpr Index offset = os;
};

// reduction operations
namespace StatOp
{
    // general templates
    template <typename T>
    inline T prod(const T &a, const T &b);
    template <typename T>
    inline T tensProd(const T &v1, const T &v2);
    template <typename T>
    inline T sum(const T &a, const T &b);
}

// Sample types
const int central = -1;

template <typename T>
using Sample = StatArray<T, 1>;

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
T StatArray<T, os>::meanOld(const Index pos, const Index n) const
{
    T           result = T();
    const Index m = (n >= 0) ? n : size();

    if (m)
    {
        result = this->segment(pos+os, m).redux(&StatOp::sum<T>);
    }
    return result/static_cast<double>(m);
}

template <typename T, Index os>
T StatArray<T, os>::sum(const Index pos, const Index n) const
{
    T           result;
    const Index m = (n >= 0) ? n : size();

    result = (*this)[pos];
    for (Index i = pos + 1; i < pos + m; ++i)
    {
        result += (*this)[i];
    }

    return result;
}

template <typename T, Index os>
T StatArray<T, os>::mean(const Index pos, const Index n) const
{
    const Index m = (n >= 0) ? n : size();

    return sum(pos, n)/static_cast<double>(m);
}

template <typename T, Index os>
T StatArray<T, os>::covarianceOld(const StatArray<T, os> &array) const
{
    T           s1, s2, prs, res = T();
    const Index m = size();
    
    if (m)
    {
        auto arraySeg = array.segment(os, m);
        auto thisSeg  = this->segment(os, m);
        
        s1  = thisSeg.redux(&StatOp::sum<T>);
        s2  = arraySeg.redux(&StatOp::sum<T>);
        prs = thisSeg.binaryExpr(arraySeg, &StatOp::prod<T>)
                     .redux(&StatOp::sum<T>);
        res = prs - StatOp::prod(s1, s2)/static_cast<double>(m);
    }
    
    return res/static_cast<double>(m - 1);
}

template <typename T, Index os>
T StatArray<T, os>::covariance(const StatArray<T, os> &array) const
{
    T s1, s2, res;

    s1  = array.sum();
    s2  = this->sum();
    res = StatOp::prod<T>(array[0], (*this)[0]);
    for (Index i = 1; i < size(); ++i)
    {
        res += StatOp::prod<T>(array[i], (*this)[i]);
    }
    res -= StatOp::prod<T>(s1, s2)/static_cast<double>(size());
    res /= static_cast<double>(size() - 1);

    return res;
}

template <typename T, Index os>
T StatArray<T, os>::covarianceMatrixOld(const StatArray<T, os> &array,
                                     const Index pos, const Index n) const
{
    T           s1, s2, prs, res = T();
    const Index m = (n >= 0) ? n : size();
    
    if (m)
    {
        auto arraySeg = array.segment(pos+os, m);
        auto thisSeg  = this->segment(pos+os, m);

        s1  = thisSeg.redux(&StatOp::sum<T>);
        s2  = arraySeg.redux(&StatOp::sum<T>);
        prs = thisSeg.binaryExpr(arraySeg, &StatOp::tensProd<T>)
                     .redux(&StatOp::sum<T>);
        res = prs - StatOp::tensProd(s1, s2)/static_cast<double>(m);
    }
    
    return res/static_cast<double>(m - 1);
}

template <typename T, Index os>
T StatArray<T, os>::variance(void) const
{
    return covariance(*this);
}

template <typename T, Index os>
T StatArray<T, os>::varianceOld(void) const
{
    return covarianceOld(*this);
}

template <typename T, Index os>
T StatArray<T, os>::varianceMatrixOld(const Index pos, const Index n) const
{
    return covarianceMatrixOld(*this, pos, n);
}

template <typename T, Index os>
T StatArray<T, os>::correlationMatrixOld(const Index pos, const Index n) const
{
    T res = varianceMatrixOld(pos, n);
    T invDiag(res.rows(), 1);

    invDiag = res.diagonal();
    invDiag = invDiag.cwiseInverse().cwiseSqrt();
    res     = (invDiag*invDiag.transpose()).cwiseProduct(res);

    return res;
}

// reduction operations ////////////////////////////////////////////////////////
namespace StatOp
{
    template <typename T>
    inline void zero(T &a)
    {
        a = 0.;
    }

    template <typename T>
    inline T sum(const T &a, const T &b)
    {
        return a + b;
    }

    template <typename T>
    inline T prod(const T &a, const T &b)
    {
        return a*b;
    }

    template <typename T>
    inline T tensProd(const T &v1 __dumb, const T &v2 __dumb)
    {
        LATAN_ERROR(Implementation,
                    "tensorial product not implemented for this type");
    }

    template <>
    inline Mat<double> prod(const Mat<double>  &a, const Mat<double>  &b)
    {
        return a.cwiseProduct(b);
    }

    template <>
    inline Mat<double> tensProd(const Mat<double>  &v1,
                                const Mat<double>  &v2)
    {
        if ((v1.cols() != 1) or (v2.cols() != 1))
        {
            LATAN_ERROR(Size,
                        "tensorial product is only valid with column vectors");
        }
        
        return v1*v2.transpose();
    }
}

// IO type /////////////////////////////////////////////////////////////////////
template <typename T, Index os>
IoObject::IoType StatArray<T, os>::getType(void) const
{
    return IoType::noType;
}

END_LATAN_NAMESPACE

#endif // Latan_StatArray_hpp_
