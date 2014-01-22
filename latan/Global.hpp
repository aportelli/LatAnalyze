#ifndef LATAN_GLOBAL_HPP_
#define	LATAN_GLOBAL_HPP_

#include <string>
#include <latan/Eigen/Dense>

#define LATAN_BEGIN_CPPDECL namespace Latan {
#define LATAN_END_CPPDECL }

// attribute to switch off unused warnings with gcc
#ifdef __GNUC__
#define __dumb __attribute__((unused))
#else
#define __dumb
#endif

LATAN_BEGIN_CPPDECL

// Environment
namespace Env
{
    extern const std::string fullName;
    extern const std::string name;
    extern const std::string version;
}

// string conversions
template <typename T>
static T ato(std::string str)
{
    T buf;
    std::istringstream stream(str);
    
    stream >> buf;
    
    return buf;
}

template <typename T>
static std::string strfrom(T x)
{
    std::ostringstream stream;
    
    stream << x;
    
    return stream.str();
}

LATAN_END_CPPDECL

#include <latan/Exceptions.hpp>

#endif
