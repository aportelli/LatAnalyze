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
    T    mean(const Index pos = 0, const Index n = -1) const;
    T    covariance(const StatArray<T, os> &array) const;
    T    variance(void) const;
    T    covarianceMatrix(const StatArray<T, os> &data) const;
    T    varianceMatrix(void) const;
    T    correlationMatrix(void) const;

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
T StatArray<T, os>::variance(void) const
{
    return covariance(*this);
}

template <typename MatType, Index os>
MatType StatArray<MatType, os>::covarianceMatrix(
    const StatArray<MatType, os> &data) const
{
    if (((*this)[central].cols() != 1) or (data[central].cols() != 1))
    {
        LATAN_ERROR(Size, "samples have more than one column");
    }

    Index   n1 = (*this)[central].rows(), n2 = data[central].rows();
    Index   nSample = this->size();
    MatType tmp1(n1, nSample), tmp2(n2, nSample), res(n1, n2);
    MatType s1(n1, 1), s2(n2, 1), one(nSample, 1);

    one.fill(1.);
    s1.fill(0.);
    s2.fill(0.);
    for (unsigned int s = 0; s < nSample; ++s)
    {
        s1          += (*this)[s];
        tmp1.col(s)  = (*this)[s];
    }
    tmp1 -= s1*one.transpose()/static_cast<double>(nSample);
    for (unsigned int s = 0; s < nSample; ++s)
    {
        s2          += data[s];
        tmp2.col(s)  = data[s];
    }
    tmp2 -= s2*one.transpose()/static_cast<double>(nSample);
    res   = tmp1*tmp2.transpose()/static_cast<double>(nSample - 1);

    return res;
}

template <typename MatType, Index os>
MatType StatArray<MatType, os>::varianceMatrix(void) const
{
    if ((*this)[0].cols() != 1)
    {
        LATAN_ERROR(Size, "samples have more than one column");
    }

    Index   n1 = (*this)[0].rows();
    Index   nSample = this->size();
    MatType tmp1(n1, nSample), res(n1, n1);
    MatType s1(n1, 1), one(nSample, 1);

    one.fill(1.);
    s1.fill(0.);
    for (unsigned int s = 0; s < nSample; ++s)
    {
        s1          += (*this)[s];
        tmp1.col(s)  = (*this)[s];
    }
    tmp1 -= s1*one.transpose()/static_cast<double>(nSample);
    res   = tmp1*tmp1.transpose()/static_cast<double>(nSample - 1);

    return res;
}

template <typename MatType, Index os>
MatType StatArray<MatType, os>::correlationMatrix(void) const
{
    MatType res = varianceMatrix();
    MatType invDiag(res.rows(), 1);

    invDiag = res.diagonal();
    invDiag = invDiag.cwiseInverse().cwiseSqrt();
    res     = (invDiag*invDiag.transpose()).cwiseProduct(res);

    return res;
}

// reduction operations ////////////////////////////////////////////////////////
namespace StatOp
{
    template <typename T>
    inline T prod(const T &a, const T &b)
    {
        return a*b;
    }

    template <>
    inline Mat<double> prod(const Mat<double>  &a, const Mat<double>  &b)
    {
        return a.cwiseProduct(b);
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
