/*
 * Exceptions.hpp, part of LatAnalyze 3
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

#ifndef Latan_Exceptions_hpp_
#define Latan_Exceptions_hpp_

#include <stdexcept>
#ifndef LATAN_GLOBAL_HPP_
#include <LatAnalyze/Global.hpp>
#endif

#define SRC_LOC strFrom(__FUNCTION__) + " at " + strFrom(__FILE__) + ":"\
                + strFrom(__LINE__)
#define LATAN_ERROR(exc,msg) throw(Exceptions::exc(msg, SRC_LOC))
#define LATAN_WARNING(msg) \
std::cerr << Env::msgPrefix << "warning: " << msg\
          << " ("  << SRC_LOC << ")" << std::endl

#define DECL_EXC(name, base) \
class name: public base\
{\
public:\
    name(std::string msg, std::string loc);\
}

BEGIN_LATAN_NAMESPACE

namespace Exceptions
{
    // logic errors
    DECL_EXC(Logic, std::logic_error);
    DECL_EXC(Definition, Logic);
    DECL_EXC(Implementation, Logic);
    DECL_EXC(Range, Logic);
    DECL_EXC(Size, Logic);
    // runtime errors
    DECL_EXC(Runtime, std::runtime_error);
    DECL_EXC(Argument, Runtime);
    DECL_EXC(Compilation, Runtime);
    DECL_EXC(Io, Runtime);
    DECL_EXC(Memory, Runtime);
    DECL_EXC(Parsing, Runtime);
    DECL_EXC(Program, Runtime);
    DECL_EXC(Syntax, Runtime);
    DECL_EXC(System, Runtime);
}

END_LATAN_NAMESPACE

#endif // Latan_Exceptions_hpp_
