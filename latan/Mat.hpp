/*
 * Mat.hpp, part of LatAnalyze 3
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

#ifndef Latan_Mat_hpp_
#define Latan_Mat_hpp_

#include <latan/Global.hpp>
#include <latan/IOObject.hpp>

#define FOR_MAT(mat, i, j) \
for (Latan::Index j = 0; j < mat.cols(); ++j)\
for (Latan::Index i = 0; i < mat.rows(); ++i)

BEGIN_NAMESPACE

class DMat: public DMatBase, public IoObject
{
private:
    typedef DMatBase Base;
public:
    // constructors
    DMat(void) = default;
    DMat(const Index nRow, const Index nCol);
    EIGEN_EXPR_CTOR(DMat, DMat, Base, MatrixBase)
    // destructor
    virtual ~DMat(void) = default;
    // IO
    virtual IoType getType(void) const;
};

END_NAMESPACE

#endif // Latan_Mat_hpp_
