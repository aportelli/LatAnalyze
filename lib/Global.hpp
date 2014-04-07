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

// supress warning for the osbolete use of 'register' keyword in Eigen
#pragma GCC diagnostic ignored "-Wdeprecated-register"

#include <LatAnalyze/Eigen/Dense>
#include <complex>
#include <memory>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>
#include <cstdlib>

#define BEGIN_NAMESPACE namespace Latan {
#define END_NAMESPACE }

// macro utilities
#define unique_arg(...) __VA_ARGS__
#define DEBUG_VAR(x) std::cout << #x << "= "  << x << std::endl
#define DEBUG_MAT(m) std::cout << #m << "=\n" << m << std::endl

// attribute to switch off unused warnings with gcc
#ifndef __GNUC__
#define __unused
#endif

// max length for paths
#define MAX_PATH_LENGTH 512u

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

BEGIN_NAMESPACE

// Eigen type aliases //////////////////////////////////////////////////////////
const int dynamic = -1;

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

typedef MatBase<double> DMatBase;

// vector types
template <typename T>
using Vec = MatBase<T, dynamic, 1>;

typedef Vec<int>                  IVec;
typedef Vec<long int>             LVec;
typedef Vec<double>               DVec;
typedef Vec<std::complex<double>> CVec;

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
typedef MatBase<double>::Index Index;

// Placeholder type ////////////////////////////////////////////////////////////
struct PlaceHolder {};

extern PlaceHolder _;

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
    // empty function for library test
    void function(void);
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

// specialization for vectors
template<>
inline DVec strTo<DVec>(const std::string &str)
{
    DVec                res;
    std::vector<double> vbuf;
    double              buf;
    std::istringstream  stream(str);
    
    while (!stream.eof())
    {
        stream >> buf;
        vbuf.push_back(buf);
    }
    res = Map<DVec>(vbuf.data(), static_cast<Index>(vbuf.size()));
    
    return res;
}

template<>
inline IVec strTo<IVec>(const std::string &str)
{
    IVec                res;
    std::vector<int>    vbuf;
    int                 buf;
    std::istringstream  stream(str);
    
    while (!stream.eof())
    {
        stream >> buf;
        vbuf.push_back(buf);
    }
    res = Map<IVec>(vbuf.data(), static_cast<Index>(vbuf.size()));
    
    return res;
}

END_NAMESPACE

#include <LatAnalyze/Exceptions.hpp>

#endif // Latan_Global_hpp_
