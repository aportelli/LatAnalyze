/*
 * MatSample.hpp, part of LatAnalyze 3
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

#ifndef Latan_MatSample_hpp_
#define Latan_MatSample_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Mat.hpp>
#include <LatAnalyze/StatArray.hpp>
#include <functional>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                          matrix sample class                               *
 ******************************************************************************/
#define SCAL_OP_RETURN(op, s, x) s.unaryExpr(\
                                    std::bind(MatSample<T>::scalar##op,\
                                    std::placeholders::_1, x))

template <typename T>
class MatSample: public Sample<Mat<T>>, public IoObject
{
public:
    // block type template
    template <class S>
    class BlockTemplate
    {
    private:
        typedef typename std::remove_const<S>::type NonConstType;
    public:
        // constructors
        BlockTemplate(S &sample, const Index i, const Index j, const Index nRow,
                      const Index nCol);
        BlockTemplate(BlockTemplate<NonConstType> &b);
        BlockTemplate(BlockTemplate<NonConstType> &&b);
        // destructor
        ~BlockTemplate(void) = default;
        // access
        S &       getSample(void);
        const S & getSample(void)   const;
        Index     getStartRow(void) const;
        Index     getStartCol(void) const;
        Index     getNRow(void)     const;
        Index     getNCol(void)     const;
        // assignement operators
        BlockTemplate<S> & operator=(const S &sample);
        BlockTemplate<S> & operator=(const S &&sample);
    private:
        S           &sample_;
        const Index i_, j_, nRow_, nCol_;
    };
    // block types
    typedef BlockTemplate<Sample<Mat<T>>>             Block;
    typedef const BlockTemplate<const Sample<Mat<T>>> ConstBlock;
public:
    // constructors
    MatSample(void) = default;
    MatSample(const Index nSample);
    MatSample(const Index nSample, const Index nRow, const Index nCol);
    MatSample(ConstBlock &sampleBlock);
    MatSample(ConstBlock &&sampleBlock);
    EIGEN_EXPR_CTOR(MatSample, MatSample<T>, Sample<Mat<T>>, ArrayExpr)
    // destructor
    virtual ~MatSample(void) = default;
    // assignement operator
    MatSample<T> & operator=(Block &sampleBlock);
    MatSample<T> & operator=(Block &&sampleBlock);
    MatSample<T> & operator=(ConstBlock &sampleBlock);
    MatSample<T> & operator=(ConstBlock &&sampleBlock);
    // product/division by scalar operators (not provided by Eigen)
    static inline Mat<T> scalarMul(const Mat<T> &m, const T &x)
    {
        return m*x;
    }
    static inline Mat<T> scalarDiv(const Mat<T> &m, const T &x)
    {
        return m/x;
    }
    MatSample<T> & operator*=(const T &x);
    MatSample<T> & operator*=(const T &&x);
    MatSample<T> & operator/=(const T &x);
    MatSample<T> & operator/=(const T &&x);
    // block access
    ConstBlock block(const Index i, const Index j, const Index nRow,
                     const Index nCol) const;
    Block      block(const Index i, const Index j, const Index nRow,
                     const Index nCol);
    // resize all matrices
    void resizeMat(const Index nRow, const Index nCol);
    // IO type
    virtual IoType getType(void) const;

};

// non-member operators
template <typename T>
inline auto operator*(MatSample<T> s, const T &x)
                      ->decltype(SCAL_OP_RETURN(Mul, s, x))
{
    return SCAL_OP_RETURN(Mul, s, x);
}

template <typename T>
inline auto operator*(MatSample<T> s, const T &&x)
                      ->decltype(SCAL_OP_RETURN(Mul, s, x))
{
    return SCAL_OP_RETURN(Mul, s, x);
}

template <typename T>
inline auto operator*(const T &x, MatSample<T> s)->decltype(s*x)
{
    return s*x;
}

template <typename T>
inline auto operator*(const T &&x, MatSample<T> s)->decltype(s*x)
{
    return s*x;
}

template <typename T>
inline auto operator/(MatSample<T> s, const T &x)
                      ->decltype(SCAL_OP_RETURN(Div, s, x))
{
    return SCAL_OP_RETURN(Div, s, x);
}

template <typename T>
inline auto operator/(MatSample<T> s, const T &&x)
                      ->decltype(SCAL_OP_RETURN(Div, s, x))
{
    return SCAL_OP_RETURN(Div, s, x);
}

// type aliases
typedef MatSample<double>               DMatSample;
typedef MatSample<std::complex<double>> CMatSample;

/******************************************************************************
 *                      Block template implementation                         *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <typename T>
template <class S>
MatSample<T>::BlockTemplate<S>::BlockTemplate(S &sample, const Index i,
                                              const Index j, const Index nRow,
                                              const Index nCol)
: sample_(sample)
, i_(i)
, j_(j)
, nRow_(nRow)
, nCol_(nCol)
{}

template <typename T>
template <class S>
MatSample<T>::BlockTemplate<S>::BlockTemplate(BlockTemplate<NonConstType> &b)
: sample_(b.getSample())
, i_(b.getStartRow())
, j_(b.getStartCol())
, nRow_(b.getNRow())
, nCol_(b.getNCol())
{}

template <typename T>
template <class S>
MatSample<T>::BlockTemplate<S>::BlockTemplate(BlockTemplate<NonConstType> &&b)
: BlockTemplate(b)
{}

// access //////////////////////////////////////////////////////////////////////
template <typename T>
template <class S>
S & MatSample<T>::BlockTemplate<S>::getSample(void)
{
    return sample_;
}

template <typename T>
template <class S>
const S & MatSample<T>::BlockTemplate<S>::getSample(void) const
{
    return sample_;
}

template <typename T>
template <class S>
Index MatSample<T>::BlockTemplate<S>::getStartRow(void) const
{
    return i_;
}

template <typename T>
template <class S>
Index MatSample<T>::BlockTemplate<S>::getStartCol(void) const
{
    return j_;
}

template <typename T>
template <class S>
Index MatSample<T>::BlockTemplate<S>::getNRow(void) const
{
    return nRow_;
}

template <typename T>
template <class S>
Index MatSample<T>::BlockTemplate<S>::getNCol(void) const
{
    return nCol_;
}

// assignement operators ///////////////////////////////////////////////////////
template <typename T>
template <class S>
MatSample<T>::BlockTemplate<S> &
MatSample<T>::BlockTemplate<S>::operator=(const S &sample)
{
    FOR_STAT_ARRAY(sample_, s)
    {
        sample_[s].block(i_, j_, nRow_, nCol_) = sample[s];
    }
    
    return *this;
}

template <typename T>
template <class S>
MatSample<T>::BlockTemplate<S> &
MatSample<T>::BlockTemplate<S>::operator=(const S &&sample)
{
    *this = sample;
    
    return *this;
}

/******************************************************************************
 *                        DMatSample implementation                           *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <typename T>
MatSample<T>::MatSample(const Index nSample)
: Sample<Mat<T>>(nSample)
{}

template <typename T>
MatSample<T>::MatSample(const Index nSample, const Index nRow,
                       const Index nCol)
: MatSample(nSample)
{
    resizeMat(nRow, nCol);
}

template <typename T>
MatSample<T>::MatSample(ConstBlock &sampleBlock)
: MatSample(sampleBlock.getSample().size(), sampleBlock.getNRow(),
            sampleBlock.getNCol())
{
    const MatSample<T> &sample = sampleBlock.getSample();
    
    this->resize(sample.size());
    FOR_STAT_ARRAY(*this, s)
    {
        (*this)[s] = sample[s].block(sampleBlock.getStartRow(),
                                     sampleBlock.getStartCol(),
                                     sampleBlock.getNRow(),
                                     sampleBlock.getNCol());
    }
}

template <typename T>
MatSample<T>::MatSample(ConstBlock &&sampleBlock)
: MatSample(sampleBlock)
{}

// assignement operator ////////////////////////////////////////////////////////
template <typename T>
MatSample<T> & MatSample<T>::operator=(Block &sampleBlock)
{
    MatSample<T> tmp(sampleBlock);
    
    this->swap(tmp);
    
    return *this;
}

template <typename T>
MatSample<T> & MatSample<T>::operator=(Block &&sampleBlock)
{
    *this = sampleBlock;
    
    return *this;
}

template <typename T>
MatSample<T> & MatSample<T>::operator=(ConstBlock &sampleBlock)
{
    MatSample<T> tmp(sampleBlock);
    
    this->swap(tmp);
    
    return *this;
}

template <typename T>
MatSample<T> & MatSample<T>::operator=(ConstBlock &&sampleBlock)
{
    *this = sampleBlock;
    
    return *this;
}

// product/division by scalar operators (not provided by Eigen) ////////////////
template <typename T>
MatSample<T> & MatSample<T>::operator*=(const T &x)
{
    return *this = (*this)*x;
}

template <typename T>
MatSample<T> & MatSample<T>::operator*=(const T &&x)
{
    return *this = (*this)*x;
}

template <typename T>
MatSample<T> & MatSample<T>::operator/=(const T &x)
{
    return *this = (*this)/x;
}

template <typename T>
MatSample<T> & MatSample<T>::operator/=(const T &&x)
{
    return *this = (*this)/x;
}

// block access ////////////////////////////////////////////////////////////////
template <typename T>
typename MatSample<T>::ConstBlock MatSample<T>::block(const Index i,
                                                      const Index j,
                                                      const Index nRow,
                                                      const Index nCol) const
{
    return ConstBlock(*this, i, j, nRow, nCol);
}

template <typename T>
typename MatSample<T>::Block MatSample<T>::block(const Index i,
                                                 const Index j,
                                                 const Index nRow,
                                                 const Index nCol)
{
    return Block(*this, i, j, nRow, nCol);
}

// resize all matrices /////////////////////////////////////////////////////////
template <typename T>
void MatSample<T>::resizeMat(const Index nRow, const Index nCol)
{
    FOR_STAT_ARRAY(*this, s)
    {
        (*this)[s].resize(nRow, nCol);
    }
}

// IO type /////////////////////////////////////////////////////////////////////
template <typename T>
IoObject::IoType MatSample<T>::getType(void) const
{
    return IoType::noType;
}

END_LATAN_NAMESPACE

#endif // Latan_MatSample_hpp_
