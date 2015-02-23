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

#include <LatCore/LatCore.hpp>
#include <complex>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>
#include <cstdlib>

#define BEGIN_LATAN_NAMESPACE \
namespace Latan {\
using namespace LatCore;
#define END_LATAN_NAMESPACE }

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

#include <LatAnalyze/Exceptions.hpp>

#endif // Latan_Global_hpp_
