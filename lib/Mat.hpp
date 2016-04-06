/*
 * Mat.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
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

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/IoObject.hpp>

#define FOR_MAT(mat, i, j) \
for (Latan::Index j = 0; j < mat.cols(); ++j)\
for (Latan::Index i = 0; i < mat.rows(); ++i)

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                               matrix type                                  *
 ******************************************************************************/
template <typename T>
class Mat: public MatBase<T>, public IoObject
{
public:
    // constructors
    Mat(void) = default;
    Mat(const Index nRow, const Index nCol);
    EIGEN_EXPR_CTOR(Mat, Mat<T>, MatBase<T>, MatExpr)
    // destructor
    virtual ~Mat(void) = default;
    // IO
    virtual IoType getType(void) const;
};

// type aliases
typedef Mat<int>                  IMat;
typedef Mat<long int>             LMat;
typedef Mat<double>               DMat;
typedef Mat<std::complex<double>> CMat;

/******************************************************************************
 *                        Mat template implementation                         *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <typename T>
Mat<T>::Mat(const Index nRow, const Index nCol)
: MatBase<T>(nRow, nCol)
{}

// IO //////////////////////////////////////////////////////////////////////////
template <typename T>
IoObject::IoType Mat<T>::getType(void) const
{
    return IoType::noType;
}

END_LATAN_NAMESPACE

#endif // Latan_Mat_hpp_
