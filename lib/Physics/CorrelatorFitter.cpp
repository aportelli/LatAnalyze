/*
 * CorrelatorFitter.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Physics/CorrelatorFitter.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                           Correlator models                                *
 ******************************************************************************/
DoubleModel CorrelatorModels::makeExpModel(const Index nState)
{
    DoubleModel mod;

    mod.setFunction([nState](const double *x, const double *p)
    {
        double res = 0.;

        for (unsigned int i = 0; i < nState; ++i)
        {
            res += p[2*i + 1]*exp(-p[2*i]*x[0]);
        }

        return res;
    }, 1, 2*nState);
    for (unsigned int i = 0; i < nState; ++i)
    {
        mod.parName().setName(2*i, "E_" + strFrom(i));
        mod.parName().setName(2*i + 1, "Z_" + strFrom(i));
    }

    return mod;
}

DoubleModel CorrelatorModels::makeCoshModel(const Index nState, const Index nt)
{
    DoubleModel mod;

    mod.setFunction([nState, nt](const double *x, const double *p)
    {
        double res = 0.;

        for (unsigned int i = 0; i < nState; ++i)
        {
            res += p[2*i + 1]*(exp(-p[2*i]*x[0]) + exp(-p[2*i]*(nt - x[0])));
        }

        return res;
    }, 1, 2*nState);
    for (unsigned int i = 0; i < nState; ++i)
    {
        mod.parName().setName(2*i, "E_" + strFrom(i));
        mod.parName().setName(2*i + 1, "Z_" + strFrom(i));
    }

    return mod;
}

DoubleModel CorrelatorModels::makeSinhModel(const Index nState, const Index nt)
{
    DoubleModel mod;

    mod.setFunction([nState, nt](const double *x, const double *p)
    {
        double res = 0.;

        for (unsigned int i = 0; i < nState; ++i)
        {
            res += p[2*i + 1]*(exp(-p[2*i]*x[0]) - exp(-p[2*i]*(nt - x[0])));
        }

        return res;
    }, 1, 2*nState);
    for (unsigned int i = 0; i < nState; ++i)
    {
        mod.parName().setName(2*i, "E_" + strFrom(i));
        mod.parName().setName(2*i + 1, "Z_" + strFrom(i));
    }

    return mod;
}

DoubleModel CorrelatorModels::makeConstModel(void)
{
    DoubleModel mod;

    mod.setFunction([](const double *x __dumb, const double *p __dumb)
    {
        return p[0];
    }, 1, 1);
    mod.parName().setName(0, "cst");

    return mod;
}

DoubleModel CorrelatorModels::makeLinearModel(void)
{
    DoubleModel mod;

    mod.setFunction([](const double *x, const double *p)
    {
        return p[1] + p[0]*x[0];
    }, 1, 2);

    return mod;
}

DoubleModel CorrelatorModels::makeExpConstModel(const Index nState)  //for constant thermal effects
{
    DoubleModel mod;

    mod.setFunction([nState](const double *x, const double *p)
    {
        double res = 0.;
        for (unsigned int i = 0; i < nState; ++i)
        {
            res += p[2*i + 1]*exp(-p[2*i]*x[0]);
        }
        res += p[2*nState]; //constant thermal effect
        return res;
    }, 1, 2*nState+1);
    for (unsigned int i = 0; i < nState; ++i)
    {
        mod.parName().setName(2*i, "E_" + strFrom(i));
        mod.parName().setName(2*i + 1, "Z_" + strFrom(i));
    }
    mod.parName().setName(2*nState, "B");

    return mod;
}

DoubleModel CorrelatorModels::makeExp2ComplementaryModel(const int t0)
{
    DoubleModel mod;

    mod.setFunction([t0](const double *x, const double *p)
    {
        double res = (1.-p[1]) * exp(-p[0]*(x[0]-t0)) + p[1] * exp(-p[2]*(x[0]-t0));

        return res;
    }, 1, 3);

    mod.parName().setName(0, "E_0");
    mod.parName().setName(1, "Z_1");
    mod.parName().setName(2, "E_1");

    return mod;
}

DoubleModel CorrelatorModels::makeExpGeometricModel(void)    // crazy summed excited states (Brett's thesis)
{
    DoubleModel mod;

    mod.setFunction([](const double *x, const double *p)
    {
        double res = 0.;
        res += p[1]*exp(-p[0]*x[0]) / ( 1. - p[3]*exp(-p[2]*x[0]) );
        return res;
    }, 1, 4);

    mod.parName().setName(0, "E_0");
    mod.parName().setName(1, "Z_1");
    mod.parName().setName(2, "delta^2");
    mod.parName().setName(3, "Z_delta");

    return mod;
}

CorrelatorModels::ModelPar CorrelatorModels::parseModel(const string s)
{
    smatch   sm;
    ModelPar par;

    if (regex_match(s, sm, regex("exp([0-9]+)")))
    {
        par.type   = CorrelatorType::exp;
        par.nState = strTo<Index>(sm[1].str());
    }
    else if (regex_match(s, sm, regex("cosh([0-9]+)")))
    {
        par.type   = CorrelatorType::cosh;
        par.nState = strTo<Index>(sm[1].str());
    }
    else if (regex_match(s, sm, regex("sinh([0-9]+)")))
    {
        par.type   = CorrelatorType::sinh;
        par.nState = strTo<Index>(sm[1].str());
    }
    else if (s == "linear")
    {
        par.type   = CorrelatorType::linear;
        par.nState = 1;
    }
    else if (s == "cst")
    {
        par.type   = CorrelatorType::cst;
        par.nState = 1;
    }
    else if (regex_match(s, sm, regex("exp([0-9]+)const")))
    {
        par.type   = CorrelatorType::exp_const;
        par.nState = strTo<Index>(sm[1].str());
    }
    else if (s== "exp2complement")
    {
        par.type   = CorrelatorType::exp2_complementary;
        par.nState = 1; // only one complete pair (E,Z)
    }
    else if (s== "summedexp")
    {
        par.type   = CorrelatorType::exp_geometric;
        par.nState = 2;
    }
    else
    {
        par.type   = CorrelatorType::undefined;
        par.nState = 0;
    }

    return par;
}

DoubleModel CorrelatorModels::makeModel(const CorrelatorModels::ModelPar par,
                                        const Index nt, const int t0)
{
    switch (par.type)
    {
    case CorrelatorType::undefined:
        LATAN_ERROR(Argument, "correlator type is undefined");
        break;
    case CorrelatorType::exp:
        return makeExpModel(par.nState);
        break;
    case CorrelatorType::cosh:
        return makeCoshModel(par.nState, nt);
        break;
    case CorrelatorType::sinh:
        return makeSinhModel(par.nState, nt);
        break;
    case CorrelatorType::linear:
        return makeLinearModel();
        break;
    case CorrelatorType::cst:
        return makeConstModel();
        break;
    case CorrelatorType::exp_const:
        return makeExpConstModel(par.nState);
        break;
    case CorrelatorType::exp2_complementary:
        return makeExp2ComplementaryModel(t0);
        break;
    case CorrelatorType::exp_geometric:
        return makeExpGeometricModel();
        break;
    }
}

DVec CorrelatorModels::parameterGuess(const DMatSample &corr, 
                                      const ModelPar par)
{
    DVec  init;
    Index nt = corr[central].size();

    switch (par.type)
    {
    case CorrelatorType::undefined:
        LATAN_ERROR(Argument, "correlator type is undefined");
        break;
    case CorrelatorType::exp:
    case CorrelatorType::cosh:
    case CorrelatorType::sinh:
    case CorrelatorType::exp2_complementary:
    case CorrelatorType::exp_geometric:
    case CorrelatorType::exp_const:
        init.resize(2*par.nState);
        init(0) = abs(log(corr[central](nt/4)/corr[central](nt/4 + 1)));
        init(1) = corr[central](nt/4)/(exp(-init(0)*nt/4));
        for (Index p = 2; p < init.size(); p += 2)
        {
            init(p)     = 2*init(p - 2);
            init(p + 1) = init(p - 1)/2.;
        }
        
        if(par.type == CorrelatorType::exp2_complementary)
        {
            init.conservativeResize(3);    
            init(2) = init(0)/2.;
        }
        else if(par.type == CorrelatorType::exp_const)
        {
            init.conservativeResize(init.size()+1);
            init(init.size()-1) = init(0)/(par.nState+1.);
        }
        break;
    case CorrelatorType::linear:
        init.resize(2);
        init(0) = corr[central](nt/4) - corr[central](nt/4 + 1, 0);
        init(1) = corr[central](nt/4, 0) + nt/4*init(0);
        break;
    case CorrelatorType::cst:
        init.resize(1);
        init(0) = corr[central](nt/8);
        break;
    default:
        break;
    }

    return init;
}

/******************************************************************************
 *                         Correlator utilities                               *
 ******************************************************************************/
DMatSample CorrelatorUtils::shift(const DMatSample &c, const Index ts)
{
    if (ts != 0)
    {
        const Index nt  = c[central].rows();
        DMatSample  buf = c;

        FOR_STAT_ARRAY(buf, s)
        {
            for (Index t = 0; t < nt; ++t)
            {
                buf[s]((t - ts + nt)%nt) = c[s](t);
            }
        }

        return buf;
    }
    else
    {
        return c;
    }
}

DMatSample CorrelatorUtils::fold(const DMatSample &c)
{
    const Index nt  = c[central].rows();
    DMatSample  buf = c;

    FOR_STAT_ARRAY(buf, s)
    {
        for (Index t = 0; t < nt; ++t)
        {
            buf[s](t) = 0.5*(c[s](t) + c[s]((nt - t) % nt));
        }
    }

    return buf;
}

DMatSample CorrelatorUtils::fourierTransform(const DMatSample &c, FFT &fft, 
                                             const unsigned int dir)
{
    const Index nSample   = c.size();
    const Index nt        = c[central].rows();
    bool        isComplex = (c[central].cols() > 1);
    CMatSample  buf(nSample, nt, 1);
    DMatSample  out(nSample, nt, 2);

    fft.resize(nt);
    FOR_STAT_ARRAY(buf, s)
    {
        buf[s].real() = c[s].col(0);
        if (isComplex)
        {
            buf[s].imag() = c[s].col(1);
        }
        else
        {
            buf[s].imag() = DVec::Constant(nt, 0.);
        }
        fft(buf[s], dir);
        out[s].col(0) = buf[s].real();
        out[s].col(1) = buf[s].imag();
    }

    return out;    
}

/******************************************************************************
 *                      CorrelatorFitter implementation                       *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
CorrelatorFitter::CorrelatorFitter(void){}

CorrelatorFitter::CorrelatorFitter(const DMatSample &corr)
{
    setCorrelator(corr);
}

CorrelatorFitter::CorrelatorFitter(const std::vector<DMatSample> &corr)
{
    setCorrelators(corr);
}

// access //////////////////////////////////////////////////////////////////////
XYSampleData & CorrelatorFitter::data(void)
{
    return *data_;
}

void CorrelatorFitter::setCorrelator(const DMatSample &corr)
{
    std::vector<DMatSample> vec;

    vec.push_back(corr);
    setCorrelators(vec);
}   

void CorrelatorFitter::setCorrelators(const std::vector<DMatSample> &corr)
{
    Index                           nSample = corr[0].size();
    DMatSample                      tVec(nSample);
    std::vector<const DMatSample *> ptVec;

    nt_ = corr[0][central].rows();
    tVec.fill(DVec::LinSpaced(nt_, 0, nt_ - 1));
    for (auto &c: corr)
    {
        ptVec.push_back(&c);
    }
    data_.reset(new XYSampleData(corr[0].size()));
    data_->addXDim(nt_, "t/a", true);
    for (unsigned int i = 0; i < corr.size(); ++i)
    {
        data_->addYDim("C_" + strFrom(i) + "(t)");
    }
    data_->setUnidimData(tVec, ptVec);
    model_.resize(corr.size());
    range_.resize(corr.size(), make_pair(0, nt_ - 1));
    thinning_.resize(corr.size(), 1);
}

void CorrelatorFitter::setModel(const DoubleModel &model, const Index i)
{
    model_[i] = model;
}

const DoubleModel & CorrelatorFitter::getModel(const Index i) const
{
    return model_.at(i);
}

void CorrelatorFitter::setFitRange(const Index tMin, const Index tMax, 
                                   const Index i)
{
    range_[i] = make_pair(tMin, tMax);
    refreshRanges();
}

void CorrelatorFitter::setCorrelation(const bool isCorrelated, const Index i, 
                                      const Index j)
{
    data_->assumeYYCorrelated(isCorrelated, i, j);
}

DMat CorrelatorFitter::getVarianceMatrix(void) const
{
    return data_->getFitVarMat();
}

void CorrelatorFitter::setThinning(const Index thinning, const Index i)
{
    thinning_[i] = thinning;
    refreshRanges();
}

// fit functions ///////////////////////////////////////////////////////////////
SampleFitResult CorrelatorFitter::fit(Minimizer &minimizer, const DVec &init,
                                  const bool onlyCentral)
{
    vector<Minimizer *> vecPt = {&minimizer};

    return fit(vecPt, init, onlyCentral);
}

SampleFitResult CorrelatorFitter::fit(vector<Minimizer *> &minimizer,
                                        const DVec &init,
                                        const bool onlyCentral)
{
    vector<const DoubleModel *> vecPt(model_.size());
    
    for (unsigned int i = 0; i < model_.size(); ++i)
    {
        vecPt[i] = &(model_[i]);
    }

    return data_->fit(minimizer, init, vecPt, onlyCentral);
}

// internal function to refresh fit ranges /////////////////////////////////////
void CorrelatorFitter::refreshRanges(void)
{
    for (unsigned int i = 0; i < range_.size(); ++i)
    for (Index t = 0; t < nt_; ++t)
    {
        data_->fitPoint((t >= range_[i].first) and (t <= range_[i].second)
                         and ((t - range_[i].first) % thinning_[i] == 0), t);
    }
}
