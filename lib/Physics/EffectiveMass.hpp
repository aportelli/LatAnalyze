/*
 * EffectiveMass.hpp, part of LatAnalyze 3
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

#ifndef Latan_EffectiveMass_hpp_
#define Latan_EffectiveMass_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>
#include <LatAnalyze/Physics/CorrelatorFitter.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                          Effective mass class                              *
 ******************************************************************************/
class EffectiveMass
{
public:
    // constructors
    EffectiveMass(const CorrelatorType type = CorrelatorType::exp);
    // access
    CorrelatorType getType(void) const;
    void           setType(const CorrelatorType type);
    // compute effective mass
    DVec       operator()(const DVec &corr) const;
    DMatSample operator()(const DMatSample &corr) const;
private:
    CorrelatorType type_;
};

END_LATAN_NAMESPACE

#endif // Latan_EffectiveMass_hpp_