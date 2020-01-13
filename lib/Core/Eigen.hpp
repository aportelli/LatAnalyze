/*
 * Eigen.hpp, part of LatAnalyze 3
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

// Eigen inclusion
#define EIGEN_DONT_PARALLELIZE
#define EIGEN_MATRIXBASE_PLUGIN <LatAnalyze/Core/EigenPlugin.hpp>
#include <LatAnalyze/Eigen/Dense>

// copy/assignement from Eigen expression
#define EIGEN_EXPR_CTOR(ctorName, Class, Base, ExprType) \
template <typename Derived>\
ctorName(const ExprType<Derived> &m): Base(m) {}\
template<typename Derived>\
Class & operator=(const ExprType<Derived> &m)\
{\
    this->Base::operator=(m);\
    return *this;\
}

#define FOR_MAT(mat, i, j) \
for (Latan::Index j = 0; j < mat.cols(); ++j)\
for (Latan::Index i = 0; i < mat.rows(); ++i)

BEGIN_LATAN_NAMESPACE

const int dynamic = Eigen::Dynamic;

// array types
template <typename Derived>
using ArrayExpr = Eigen::ArrayBase<Derived>;

template <typename T, int nRow = dynamic, int nCol = dynamic>
using Array = Eigen::Array<T, nRow, nCol>;

// matrix types
template <typename Derived>
using MatExpr = Eigen::MatrixBase<Derived>;

template <typename T, int nRow = dynamic, int nCol = dynamic>
using MatBase = Eigen::Matrix<T, nRow, nCol>;

template <int nRow, int nCol>
using SFMat = Eigen::Matrix<float, nRow, nCol>;

template <int nRow, int nCol>
using SDMat = Eigen::Matrix<double, nRow, nCol>;

template <int nRow, int nCol>
using SCMat = Eigen::Matrix<std::complex<double>, nRow, nCol>;

// vector types
template <typename T, int size = dynamic>
using Vec = MatBase<T, size, 1>;

template <int size>
using SIVec = Vec<int, size>;

template <int size>
using SUVec = Vec<unsigned int, size>;

template <int size>
using SFVec = Vec<float, size>;

template <int size>
using SDVec = Vec<double, size>;

template <int size>
using SCVec = Vec<std::complex<double>, size>;

typedef SIVec<dynamic> IVec;
typedef SUVec<dynamic> UVec;
typedef SDVec<dynamic> DVec;
typedef SCVec<dynamic> CVec;

// block types
template <typename Derived>
using Block      = Eigen::Block<Derived>;
template <typename Derived>
using ConstBlock = const Eigen::Block<const Derived>;

template <typename Derived>
using Row      = typename Derived::RowXpr;
template <typename Derived>
using ConstRow = typename Derived::ConstRowXpr;

template <typename Derived>
using Col = typename Derived::ColXpr;
template <typename Derived>
using ConstCol = typename Derived::ConstColXpr;

// map type
template <int stride>
using InnerStride = Eigen::InnerStride<stride>;
template <int rowStride, int colStride>
using Stride = Eigen::Stride<rowStride, colStride>;
template <typename Derived, typename StrideType = Stride<0, 0>>
using Map = Eigen::Map<Derived, Eigen::Unaligned, StrideType>;
template <typename Derived, typename StrideType = Stride<0, 0>>
using ConstMap = Eigen::Map<const Derived, Eigen::Unaligned, StrideType>;

// Index type //////////////////////////////////////////////////////////////////
typedef MatBase<int>::Index Index;

#define FOR_VEC(vec, i)  for (Latan::Index i = 0; i < (vec).size(); ++i)
#define FOR_ARRAY(ar, i) FOR_VEC(ar, i)

END_LATAN_NAMESPACE
