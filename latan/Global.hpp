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

#include <map>
#include <string>
#include <latan/Eigen/Dense>

#define BEGIN_NAMESPACE namespace Latan {
#define END_NAMESPACE }

// attribute to switch off unused warnings with gcc
#ifdef __GNUC__
#define __dumb __attribute__((unused))
#else
#define __dumb
#endif

// max length for paths
#define MAX_PATH_LENGTH 512

BEGIN_NAMESPACE

// Environment
namespace Env
{
    extern const std::string fullName;
    extern const std::string name;
    extern const std::string version;
}

// string conversions
template <typename T>
T strTo(std::string str)
{
    T buf;
    std::istringstream stream(str);
    
    stream >> buf;
    
    return buf;
}

template <typename T>
std::string strFrom(T x)
{
    std::ostringstream stream;
    
    stream << x;
    
    return stream.str();
}

// key test in maps
template <typename K, typename T>
bool keyExists(const K &key, const std::map<K, T> &map)
{
    return (map.find(key) != map.end());
}

END_NAMESPACE

#include <latan/Exceptions.hpp>

#endif // Latan_Global_hpp_
