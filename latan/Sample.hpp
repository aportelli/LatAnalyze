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
    // constructors
    DMatSample(void);
    DMatSample(const Index nSample, const Index nRow, const Index nCol);
    using Sample<DMat>::Sample;
    // destructor
    virtual ~DMatSample(void) = default;
    // resize all matrices
    void resizeMat(const Index nRow, const Index nCol);
    // IO type
    virtual IoType getType(void) const;
};

END_NAMESPACE

#endif // Latan_Sample_hpp_
