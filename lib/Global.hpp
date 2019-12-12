/*
 * Global.hpp, part of LatAnalyze 3
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

#ifndef Latan_Global_hpp_
#define	Latan_Global_hpp_

#include <LatAnalyze/Core/stdincludes.hpp>

#define BEGIN_LATAN_NAMESPACE \
namespace Latan {
#define END_LATAN_NAMESPACE }

// macro utilities
#define unique_arg(...) __VA_ARGS__
#define DEBUG_VAR(x) std::cout << #x << "= "  << x << std::endl
#define DEBUG_MAT(m) std::cout << #m << "=\n" << m << std::endl

// attribute to switch off unused warnings with gcc
#ifdef __GNUC__
#define __dumb __attribute__((unused))
#else
#define __dumb
#endif

// max length for paths
#define MAX_PATH_LENGTH 512u

BEGIN_LATAN_NAMESPACE

// Placeholder type ////////////////////////////////////////////////////////////
struct PlaceHolder {};

extern PlaceHolder _;

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

END_LATAN_NAMESPACE

#include <LatAnalyze/Core/Eigen.hpp>
#include <LatAnalyze/Core/Exceptions.hpp>
#include <LatAnalyze/Core/Utilities.hpp>

#endif // Latan_Global_hpp_
