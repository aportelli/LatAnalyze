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

const int central = -1;

/******************************************************************************
 *                              Sample class                                  *
 ******************************************************************************/
template <typename T>
class Sample: public StatArray<T>, public IoObject
{
private:
    typedef StatArray<T> Base;
public:
    // constructors
    Sample(void);
    Sample(const unsigned int nSample);
    template <typename Derived>
    Sample(const Eigen::EigenBase<Derived> &s);
    // destructor
    virtual ~Sample(void);
    // operators
    T& operator[](const int s);
    // IO type
    virtual IoType getType(void) const;
private:
    // index of the first element to take into account for statistics
    virtual unsigned int getOffset(void) const;
};

template <>
IoObject::IoType Sample<DMat>::getType(void) const;

// specialization aliases
typedef Sample<DMat> DMatSample;

/******************************************************************************
 *                    Sample class template implementation                    *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
template <typename T>
Sample<T>::Sample(void)
: Base(static_cast<typename Base::Index>(getOffset()))
{}

template <typename T>
Sample<T>::Sample(const unsigned int nSample)
: Base(static_cast<typename Base::Index>(nSample + getOffset()))
{}

template <typename T>
template <typename Derived>
Sample<T>::Sample(const  Eigen::EigenBase<Derived> &s)
: Base(s)
{}

// destructor //////////////////////////////////////////////////////////////////
template <typename T>
Sample<T>::~Sample(void)
{}

// operators ///////////////////////////////////////////////////////////////////
template <typename T>
T& Sample<T>::operator[](const int s)
{
    if (s >= 0)
    {
        return Base::operator[](s + 1);
    }
    else
    {
        return Base::operator[](0);
    }
}

// IO type /////////////////////////////////////////////////////////////////////
template <typename T>
IoObject::IoType Sample<T>::getType(void) const
{
    return IoType::noType;
}

// statistics //////////////////////////////////////////////////////////////////
template <typename T>
unsigned int Sample<T>::getOffset(void) const
{
    return 1u;
}

END_NAMESPACE

#endif // Latan_Sample_hpp_
