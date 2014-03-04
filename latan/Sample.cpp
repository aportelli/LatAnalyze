/*
 * Sample.cpp, part of LatAnalyze 3
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

#include <latan/Sample.hpp>
#include <latan/includes.hpp>

using namespace Latan;
using namespace std;

/******************************************************************************
 *                        DMatSample implementation                           *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
DMatSample::DMatSample(const Index nSample, const Index nRow,
                       const Index nCol)
: Sample<DMat>(nSample)
{
    resizeMat(nRow, nCol);
}

DMatSample::DMatSample(ConstBlock &sampleBlock)
: DMatSample(sampleBlock.getSample().size(), sampleBlock.getNRow(),
             sampleBlock.getNCol())
{
    const DMatSample &sample = sampleBlock.getSample();
    
    FOR_STAT_ARRAY(*this, s)
    {
        (*this)[s] = sample[s].block(sampleBlock.getStartRow(),
                                     sampleBlock.getStartCol(),
                                     sampleBlock.getNRow(),
                                     sampleBlock.getNCol());
    }
}

DMatSample::DMatSample(ConstBlock &&sampleBlock)
: DMatSample(sampleBlock)
{}

// assignement operator ////////////////////////////////////////////////////////
DMatSample & DMatSample::operator=(Block &sampleBlock)
{
    DMatSample tmp(sampleBlock);
    
    this->swap(tmp);
    
    return *this;
}

DMatSample & DMatSample::operator=(Block &&sampleBlock)
{
    *this = sampleBlock;
    
    return *this;
}

DMatSample & DMatSample::operator=(ConstBlock &sampleBlock)
{
    DMatSample tmp(sampleBlock);
    
    this->swap(tmp);
    
    return *this;
}

DMatSample & DMatSample::operator=(ConstBlock &&sampleBlock)
{
    *this = sampleBlock;
    
    return *this;
}

// block access ////////////////////////////////////////////////////////////////
DMatSample::ConstBlock DMatSample::block(const Index i, const Index j,
                                         const Index nRow,
                                         const Index nCol) const
{
    return ConstBlock(*this, i, j, nRow, nCol);
}

DMatSample::Block DMatSample::block(const Index i, const Index j,
                                    const Index nRow, const Index nCol)
{
    return Block(*this, i, j, nRow, nCol);
}

// resize all matrices /////////////////////////////////////////////////////////
void DMatSample::resizeMat(const Index nRow, const Index nCol)
{
    FOR_STAT_ARRAY(*this, s)
    {
        (*this)[s].resize(nRow, nCol);
    }
}

// IO type /////////////////////////////////////////////////////////////////////
IoObject::IoType DMatSample::getType(void) const
{
    return IoType::dMatSample;
}
