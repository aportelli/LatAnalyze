/*
 * Model.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli
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

#ifndef Latan_Model_hpp_
#define Latan_Model_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Function.hpp>
#include <LatAnalyze/Mat.hpp>
#include <vector>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                           Double model class                               *
 ******************************************************************************/
class DoubleModel
{
public:
    typedef std::function<double(const double *, const double *)> vecFunc;
private:
    struct ModelSize{Index nArg, nPar;};
public:
    // constructor
    DoubleModel(const vecFunc &f = nullptr, const Index nArg = 0,
                const Index nPar = 0);
    // destructor
    virtual ~DoubleModel(void) = default;
    // access
    virtual Index     getNArg(void) const;
    virtual Index     getNPar(void) const;
            void      setFunction(const vecFunc &f, const Index nArg,
                                  const Index nPar);
            VarName & varName(void);
      const VarName & varName(void) const;
            VarName & parName(void);
      const VarName & parName(void) const;
    // function call
    double operator()(const DVec &data, const DVec &par) const;
    double operator()(const std::vector<double> &data,
                      const std::vector<double> &par) const;
    double operator()(const double *data, const double *par) const;
    // bind
    DoubleFunction fixArg(const DVec &arg) const;
    DoubleFunction fixPar(const DVec &par) const;
    DoubleFunction toFunction(void) const;
private:
    // error checking
    void checkSize(const Index nArg, const Index nPar) const;
private:
    std::shared_ptr<ModelSize> size_;
    VarName                    varName_, parName_;
    vecFunc                    f_;
};

/******************************************************************************
 *                       base class for model factories                       *
 ******************************************************************************/
class DoubleModelFactory
{
public:
    // constructor
    DoubleModelFactory(void) = default;
    // destructor
    virtual ~DoubleModelFactory(void) = default;
    // factory
    virtual DoubleModel makeModel(const bool makeHardCopy) const = 0;
};

END_LATAN_NAMESPACE

#endif // Latan_Model_hpp_
