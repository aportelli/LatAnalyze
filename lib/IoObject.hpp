/*
 * IoObject.hpp, part of LatAnalyze 3
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

#ifndef Latan_IoObject_hpp_
#define Latan_IoObject_hpp_

#include <LatAnalyze/Global.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                        Abstract class for IO objects                       *
 ******************************************************************************/
class IoObject
{
public:
    enum class IoType
    {
        noType     = 0,
        dMat       = 1,
        dMatSample = 2,
        rgState    = 3
    };
public:
    // destructor
    virtual ~IoObject(void) = default;
    // access
    virtual IoType getType(void) const = 0;
};

END_LATAN_NAMESPACE

#endif // Latan_IoObject_hpp_
