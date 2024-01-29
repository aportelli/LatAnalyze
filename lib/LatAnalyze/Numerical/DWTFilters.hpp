/*
 * DWTFilters.hpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Latan_DWTFilters_hpp_
#define Latan_DWTFilters_hpp_

#include <LatAnalyze/Global.hpp>

BEGIN_LATAN_NAMESPACE

struct DWTFilter
{
    const std::vector<double> fwdL, fwdH, bwdL, bwdH;
};

namespace DWTFilters
{
    extern DWTFilter haar;
    extern DWTFilter db2;
    extern DWTFilter db3;
    extern DWTFilter db4;
    extern DWTFilter db5;
    extern DWTFilter db6;
    extern DWTFilter bior13;
    extern DWTFilter bior15;
    extern DWTFilter bior22;
    extern DWTFilter bior24;
    extern DWTFilter bior31;
    extern DWTFilter bior33;
    extern DWTFilter bior35;

    extern std::map<std::string, const DWTFilter *> fromName;
}

END_LATAN_NAMESPACE

#endif // Latan_DWTFilters_hpp_
