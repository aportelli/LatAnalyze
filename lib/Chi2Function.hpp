/*
 * Chi2Function.hpp, part of LatAnalyze 3
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

#ifndef Latan_Chi2Function_hpp_
#define Latan_Chi2Function_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Model.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                                 <Class>                                    *
 ******************************************************************************/
// forward declaration of XYStatData
class XYStatData;

class Chi2Function: public DoubleFunction
{
private:
    struct Chi2FunctionBuffer
    {
        DVec       v, x;
        DMat       invVar;
        bool       isInit{false};
        Vec<Index> xInd, dInd;
    };
public:
    // constructor
    explicit Chi2Function(const XYStatData &data);
    Chi2Function(const XYStatData &data,
                 const std::vector<const DoubleModel *> &modelVector);
    // destructor
    virtual ~Chi2Function(void) = default;
    // access
    virtual Index getNArg(void) const;
            Index getNDof(void) const;
            Index getNPar(void) const;
            void  setModel(const DoubleModel &model, const Index j = 0);
            void  setModel(const std::vector<const DoubleModel *> &modelVector);
            void  requestInit(void) const;
    // function call
    using DoubleFunction::operator();
protected:
    // function call
    virtual double operator()(const double *arg) const;
private:
    // access
    void resizeBuffer(void) const;
    
    // compute variance matrix inverse
    void setVarianceBlock(const Index l1, const Index l2,
                          ConstBlock<DMatBase> m) const;
    void initBuffer(void) const;
private:
    const XYStatData                    &data_;
    std::shared_ptr<Chi2FunctionBuffer> buffer_;
    std::vector<const DoubleModel *>    model_;
    Index                               nPar_{-1};
};

END_LATAN_NAMESPACE

#endif // Latan_Chi2Function_hpp_
