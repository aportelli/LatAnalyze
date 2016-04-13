/*
 * Exceptions.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Exceptions.hpp>
#include <LatAnalyze/includes.hpp>

#ifndef ERR_SUFF
#define ERR_SUFF " (" + loc + ")"
#endif

#define CONST_EXC(name, init) \
name::name(string msg, string loc)\
:init\
{}

using namespace std;
using namespace Latan;
using namespace Exceptions;

// logic errors
CONST_EXC(Logic, logic_error(Env::msgPrefix + msg + ERR_SUFF))
CONST_EXC(Definition, Logic("definition error: " + msg, loc))
CONST_EXC(Implementation, Logic("implementation error: " + msg, loc))
CONST_EXC(Range, Logic("range error: " + msg, loc))
CONST_EXC(Size, Logic("size error: " + msg, loc))
// runtime errors
CONST_EXC(Runtime, runtime_error(Env::msgPrefix + msg + ERR_SUFF))
CONST_EXC(Argument, Runtime("argument error: " + msg, loc))
CONST_EXC(Compilation, Runtime("compilation error: " + msg, loc))
CONST_EXC(Io, Runtime("IO error: " + msg, loc))
CONST_EXC(Memory, Runtime("memory error: " + msg, loc))
CONST_EXC(Parsing, Runtime(msg, loc))
CONST_EXC(Program, Runtime(msg, loc))
CONST_EXC(Syntax, Runtime("syntax error: " + msg, loc))
CONST_EXC(System, Runtime("system error: " + msg, loc))
