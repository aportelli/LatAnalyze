/*
 * Global.hpp, part of LatAnalyze 3
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

#ifndef Latan_Global_hpp_
#define	Latan_Global_hpp_

#include <latan/Eigen/Dense>
#include <memory>
#include <string>
#include <sstream>
#include <type_traits>
#include <cstdlib>

#define BEGIN_NAMESPACE namespace Latan {
#define END_NAMESPACE }

// macro utilities
#define unique_arg(...) __VA_ARGS__

// attribute to switch off unused warnings with gcc
#ifndef __GNUC__
#define __unused
#endif

// max length for paths
#define MAX_PATH_LENGTH 512u

// copy/assignement from Eigen expression
#define EIGEN_EXPR_CTOR(ctorName, Class, Base, EigenBase) \
template <typename Derived>\
ctorName(const Eigen::EigenBase<Derived> &m): Base(m) {}\
template<typename Derived>\
Class & operator=(const Eigen::EigenBase<Derived> &m)\
{\
    this->Base::operator=(m);\
    return *this;\
}

BEGIN_NAMESPACE

// Eigen type aliases //////////////////////////////////////////////////////////
const int dynamic = -1;

// array types
template <typename T, int nRow = dynamic, int nCol = dynamic>
using Array = Eigen::Array<T, nRow, nCol>;

// matrix types
template <typename T, int nRow = dynamic, int nCol = dynamic>
using Mat = Eigen::Matrix<T, nRow, nCol>;

typedef Mat<int>    IMat;
typedef Mat<double> DMatBase;

// vector types
template <typename T>
using Vec = Mat<T, dynamic, 1>;

typedef Vec<int>    IVec;
typedef Vec<double> DVec;

#define FOR_VEC(vec, i)  for (Latan::Index i = 0; i < (vec).size(); ++i)
#define FOR_ARRAY(ar, i) FOR_VEC(ar, i)

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
template <typename Derived>
using Map = Eigen::Map<Derived>;
template <typename Derived>
using ConstMap = Eigen::Map<const Derived>;

// Index type //////////////////////////////////////////////////////////////////
typedef DMatBase::Index Index;

// Type utilities //////////////////////////////////////////////////////////////
// pointer type test
template <typename Derived, typename Base>
inline bool isDerivedFrom(const Base *pt)
{
    return (dynamic_cast<const Derived *>(pt) != nullptr);
}

// static logical or
template <bool... b>
struct static_or;

template <bool... tail>
struct static_or<true, tail...> : static_or<tail...> {};

template <bool... tail>
struct static_or<false, tail...> : std::false_type {};

template <>
struct static_or<> : std::true_type {};

// Environment /////////////////////////////////////////////////////////////////
namespace Env
{
    extern const std::string fullName;
    extern const std::string name;
    extern const std::string version;
    extern const std::string msgPrefix;
}

// String conversions //////////////////////////////////////////////////////////
template <typename T>
inline T strTo(const std::string &str)
{
    T buf;
    std::istringstream stream(str);
    
    stream >> buf;
    
    return buf;
}

// optimized specializations
template <>
inline float strTo<float>(const std::string &str)
{
    return strtof(str.c_str(), (char **)NULL);
}
template <>
inline double strTo<double>(const std::string &str)
{
    return strtod(str.c_str(), (char **)NULL);
}
template <>
inline int strTo<int>(const std::string &str)
{
    return (int)(strtol(str.c_str(), (char **)NULL, 10));
}

template <typename T>
inline std::string strFrom(const T x)
{
    std::ostringstream stream;
    
    stream << x;
    
    return stream.str();
}

END_NAMESPACE

#include <latan/Exceptions.hpp>

#endif // Latan_Global_hpp_
