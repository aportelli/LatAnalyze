/*
 * Sample.hpp, part of LatAnalyze 3
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

#ifndef Latan_Sample_hpp_
#define	Latan_Sample_hpp_

#include <latan/Global.hpp>
#include <latan/IoObject.hpp>
#include <latan/Mat.hpp>
#include <latan/StatArray.hpp>

BEGIN_NAMESPACE

#define SAMPLE_OFFSET 1u

const int central = -static_cast<int>(SAMPLE_OFFSET);

/******************************************************************************
 *                              Sample class                                  *
 ******************************************************************************/
template <typename T>
class Sample: public StatArray<T, SAMPLE_OFFSET>
{
private:
    typedef StatArray<T, SAMPLE_OFFSET> Base;
public:
    // constructors
    Sample(void) = default;
    using Base::Base;
    // destructor
    virtual ~Sample(void) = default;
};

/******************************************************************************
 *                          DMatSample class                                  *
 ******************************************************************************/

class DMatSample: public Sample<DMat>, public IoObject
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
    typedef BlockTemplate<DMatSample>             Block;
    typedef const BlockTemplate<const DMatSample> ConstBlock;
public:
    // constructors
    DMatSample(void) = default;
    DMatSample(const Index nSample, const Index nRow, const Index nCol);
    DMatSample(ConstBlock &sampleBlock);
    DMatSample(ConstBlock &&sampleBlock);
    using Sample<DMat>::Sample;
    // destructor
    virtual ~DMatSample(void) = default;
    // assignement operator
    DMatSample & operator=(Block &sampleBlock);
    DMatSample & operator=(Block &&sampleBlock);
    DMatSample & operator=(ConstBlock &sampleBlock);
    DMatSample & operator=(ConstBlock &&sampleBlock);
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

/******************************************************************************
 *                      Block template implementation                         *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <class S>
DMatSample::BlockTemplate<S>::BlockTemplate(S &sample, const Index i,
                                            const Index j, const Index nRow,
                                            const Index nCol)
: sample_(sample)
, i_(i)
, j_(j)
, nRow_(nRow)
, nCol_(nCol)
{}

template <class S>
DMatSample::BlockTemplate<S>::BlockTemplate(BlockTemplate<NonConstType> &b)
: sample_(b.getSample())
, i_(b.getStartRow())
, j_(b.getStartCol())
, nRow_(b.getNRow())
, nCol_(b.getNCol())
{}

template <class S>
DMatSample::BlockTemplate<S>::BlockTemplate(BlockTemplate<NonConstType> &&b)
: BlockTemplate(b)
{}

// access //////////////////////////////////////////////////////////////////////
template <class S>
S & DMatSample::BlockTemplate<S>::getSample(void)
{
    return sample_;
}

template <class S>
const S & DMatSample::BlockTemplate<S>::getSample(void) const
{
    return sample_;
}

template <class S>
Index DMatSample::BlockTemplate<S>::getStartRow(void) const
{
    return i_;
}

template <class S>
Index DMatSample::BlockTemplate<S>::getStartCol(void) const
{
    return j_;
}

template <class S>
Index DMatSample::BlockTemplate<S>::getNRow(void) const
{
    return nRow_;
}

template <class S>
Index DMatSample::BlockTemplate<S>::getNCol(void) const
{
    return nCol_;
}

// assignement operators ///////////////////////////////////////////////////////
template <class S>
DMatSample::BlockTemplate<S> &
DMatSample::BlockTemplate<S>::operator=(const S &sample)
{
    FOR_STAT_ARRAY(sample_, s)
    {
        sample_[s].block(i_, j_, nRow_, nCol_) = sample[s];
    }
    
    return *this;
}

template <class S>
DMatSample::BlockTemplate<S> &
DMatSample::BlockTemplate<S>::operator=(const S &&sample)
{
    *this = sample;
    
    return *this;
}

END_NAMESPACE

#endif // Latan_Sample_hpp_
