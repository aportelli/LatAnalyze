/*
 * EffectiveMass.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Physics/EffectiveMass.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                       EffectiveMass implementation                         *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
EffectiveMass::EffectiveMass(const CorrelatorType type)
{
    setType(type);
}

// access //////////////////////////////////////////////////////////////////////
CorrelatorType EffectiveMass::getType(void) const
{
    return type_;
}

void EffectiveMass::setType(const CorrelatorType type)
{
    type_ = type;
}

DVec EffectiveMass::getTime(const Index nt) const
{
    DVec tvec;

    switch (type_)
    {
    case CorrelatorType::undefined:
        LATAN_ERROR(Argument, "correlator type is undefined");
        break;
    case CorrelatorType::exp:
    case CorrelatorType::exp_const:
    case CorrelatorType::exp2_complementary:
    case CorrelatorType::exp_geometric:
    case CorrelatorType::linear:
        tvec = DVec::LinSpaced(nt - 1, 0, nt - 2);
        break;
    case CorrelatorType::cosh:
    case CorrelatorType::sinh:
        tvec = DVec::LinSpaced(nt - 2, 1, nt - 2);
        break;
    case CorrelatorType::cst:
        tvec = DVec::LinSpaced(nt, 0, nt - 1);
        break;
    }

    return tvec;
}

// compute effective mass //////////////////////////////////////////////////////
DVec EffectiveMass::operator()(const DVec &corr) const
{
    Index nt = corr.size();
    DVec  em;

    if (nt < 2)
    {
        LATAN_ERROR(Size, "input vector has less than 2 elements");
    }
    switch (type_)
    {
    case CorrelatorType::undefined:
        LATAN_ERROR(Argument, "correlator type is undefined");
        break;
    case CorrelatorType::exp:
    case CorrelatorType::exp_const:
    case CorrelatorType::exp2_complementary:
    case CorrelatorType::exp_geometric:
        em.resize(nt - 1);
        for (Index t = 1; t < nt; ++t)
        {
            em(t - 1) = log(corr(t - 1)/corr(t));
        }
        break;
    case CorrelatorType::cosh:
        em.resize(nt - 2);
        for (Index t = 1; t < nt - 1; ++t)
        {
            em(t - 1) = acosh((corr(t - 1) + corr(t + 1))/(2.*corr(t)));
        }
        break;
    case CorrelatorType::sinh:
        em.resize(nt - 2);
        for (Index t = 1; t < nt - 1; ++t)
        {
            em(t - 1) = acosh((corr(t - 1) + corr(t + 1))/(2.*corr(t)));
        }
        break;
    case CorrelatorType::linear:
        em.resize(nt - 1);
        for (Index t = 1; t < nt; ++t)
        {
            em(t - 1) = corr(t) - corr(t - 1);
        }
        break;
    case CorrelatorType::cst:
        em = corr;
        break;
    }

    return em;
}

DMatSample EffectiveMass::operator()(const DMatSample &corr) const
{
    DMatSample em(corr.size());

    FOR_STAT_ARRAY(em, s)
    {
        em[s] = (*this)(corr[s]);
    }

    return em;
}
