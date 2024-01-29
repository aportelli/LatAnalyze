/*
 * Global.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

PlaceHolder Latan::_;

const string Env::fullName  = PACKAGE_STRING;
const string Env::name      = PACKAGE_NAME;
const string Env::version   = PACKAGE_VERSION;
const string Env::msgPrefix = "[" + strFrom(PACKAGE_NAME) + " v"
                              + strFrom(PACKAGE_VERSION) + "] ";

void Env::function(void)
{}
