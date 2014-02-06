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
#include <latan/Mat.hpp>

BEGIN_NAMESPACE

const int Central = -1;

typedef Eigen::Array<DMat, Eigen::Dynamic, 1> DSampleBase;

class DSample: public DSampleBase
{
public:
    // Constructors/destructor
    DSample(void);
    DSample(const unsigned int nSample, const unsigned int nRow,
            const unsigned int nCol);
    ~DSample(void);
    // Operators
    DMat& operator()(const int s);
};

END_NAMESPACE

#endif // Latan_Sample_hpp_
