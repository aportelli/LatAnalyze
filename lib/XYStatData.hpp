/*
 * XYStatData.hpp, part of LatAnalyze 3
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

#ifndef Latan_XYStatData_hpp_
#define Latan_XYStatData_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/FitInterface.hpp>
#include <LatAnalyze/Minimizer.hpp>
#include <LatAnalyze/Model.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                           object for fit result                            *
 ******************************************************************************/
class FitResult: public DVec
{
    friend class XYStatData;
    friend class XYSampleData;
    friend class SampleFitResult;
public:
    // constructors
    FitResult(void) = default;
    EIGEN_EXPR_CTOR(FitResult, FitResult, Base, MatExpr)
    // destructor
    virtual ~FitResult(void) = default;
    // access
    double                 getChi2(void) const;
    double                 getChi2PerDof(void) const;
    double                 getNDof(void) const;
    Index                  getNPar(void) const;
    double                 getPValue(void) const;
    const DoubleFunction & getModel(const Index j = 0) const;
    // IO
    void print(const bool printXsi = false,
               std::ostream &out = std::cout) const;
private:
    double                      chi2_{0.};
    Index                       nDof_{0}, nPar_{0};
    std::vector<DoubleFunction> model_;
    std::vector<std::string>    parName_;
};

/******************************************************************************
 *                    class for X vs. Y statistical data                      *
 ******************************************************************************/
class XYStatData: public FitInterface
{
public:
    // constructor
    XYStatData(void) = default;
    // destructor
    virtual ~XYStatData(void) = default;
    // data access
    double &       x(const Index r, const Index i);
    const double & x(const Index r, const Index i) const;
    const DVec &   x(const Index k) const;
    double &       y(const Index k, const Index j);
    const double & y(const Index k, const Index j) const;
    void           setXXVar(const Index i1, const Index i2, const DMat &m);
    void           setYYVar(const Index j1, const Index j2, const DMat &m);
    void           setXYVar(const Index i, const Index j, const DMat &m);
    void           setXError(const Index i, const DVec &err);
    void           setYError(const Index j, const DVec &err);
    template <typename... Ts>
    void           setUnidimData(const DMat &xData, const Ts & ...yDatas);
    const DMat &   getXXVar(const Index i1, const Index i2) const;
    const DMat &   getYYVar(const Index j1, const Index j2) const;
    const DMat &   getXYVar(const Index i, const Index j) const;
    DVec           getXError(const Index i) const;
    DVec           getYError(const Index j) const;
    DMat           getTable(const Index i, const Index j) const;
    // get total fit variance matrix and its pseudo-inverse
    const DMat & getFitVarMat(void);
    const DMat & getFitVarMatPInv(void);
    // fit
    FitResult fit(std::vector<Minimizer *> &minimizer, const DVec &init,
                  const std::vector<const DoubleModel *> &v);
    FitResult fit(Minimizer &minimizer, const DVec &init,
                  const std::vector<const DoubleModel *> &v);
    template <typename... Ts>
    FitResult fit(std::vector<Minimizer *> &minimizer, const DVec &init,
                  const DoubleModel &model, const Ts... models);
    template <typename... Ts>
    FitResult fit(Minimizer &minimizer, const DVec &init,
                  const DoubleModel &model, const Ts... models);
    // residuals
    XYStatData getResiduals(const FitResult &fit);
    XYStatData getPartialResiduals(const FitResult &fit, const DVec &ref,
                                   const Index i);
protected:
    // create data
    virtual void createXData(const std::string name, const Index nData);
    virtual void createYData(const std::string name);
    void         resizeVarMat(void);
private:
    // schedule buffer computation
    void scheduleXMapInit(void);
    void scheduleChi2DataVecInit(void);
    // buffer total fit variance matrix
    void updateFitVarMat(void);
    // buffer list of x vectors
    void updateXMap(void) const;
    // buffer chi^2 vectors
    void updateChi2DataVec(void);
    void updateChi2ModVec(const DVec p,
                          const std::vector<const DoubleModel *> &v);
private:
    std::vector<std::map<Index, double>> yData_;
    std::vector<DVec>                    xData_;
    std::map<Index, DVec>                xMap_;
    Mat<DMat>                            xxVar_, yyVar_, xyVar_;
    DMat                                 fitVar_, fitVarInv_;
    DVec                                 chi2DataVec_, chi2ModVec_, chi2Vec_;
    DVec                                 xBuf_;
    bool                                 initXMap_{true};
    bool                                 initChi2DataVec_{true};
};

/******************************************************************************
 *                     XYStatData template implementation                     *
 ******************************************************************************/
template <typename... Ts>
void XYStatData::setUnidimData(const DMat &xData, const Ts & ...yDatas)
{
    static_assert(static_or<std::is_assignable<DMat, Ts>::value...>::value,
                  "y data arguments are not compatible with DMat");
    
    std::vector<const DMat *> yData{&yDatas...};
    
    FOR_VEC(xData, r)
    {
        x(r, 0) = xData(r);
        for (unsigned int j = 0; j < yData.size(); ++j)
        {
            y(r, j) = (*(yData[j]))(r);
        }
    }
}

template <typename... Ts>
FitResult XYStatData::fit(std::vector<Minimizer *> &minimizer, const DVec &init,
                          const DoubleModel &model, const Ts... models)
{
    static_assert(static_or<std::is_assignable<DoubleModel &, Ts>::value...>::value,
                  "model arguments are not compatible with DoubleModel");
    
    std::vector<const DoubleModel *> modelVector{&model, &models...};
    
    return fit(minimizer, init, modelVector);
}

template <typename... Ts>
FitResult XYStatData::fit(Minimizer &minimizer, const DVec &init,
                          const DoubleModel &model, const Ts... models)
{
    static_assert(static_or<std::is_assignable<DoubleModel &, Ts>::value...>::value,
                  "model arguments are not compatible with DoubleModel");
    
    std::vector<Minimizer *> mv{&minimizer};
    
    return fit(mv, init, model, models...);
}

/******************************************************************************
 *                       error check macros                                   *
 ******************************************************************************/
#define checkVarMat(m, var)\
if (((m).rows() != (var).rows()) or ((m).cols() != (var).cols()))\
{\
    LATAN_ERROR(Size, "provided variance matrix has a wrong size"\
                " (expected " + strFrom((var).rows()) + "x"\
                + strFrom((var).cols()) + ", got " + strFrom((m).rows())\
                + "x" + strFrom((m).cols()) + ")");\
}


#define checkErrVec(err, var)\
if ((err).size() != (var).rows())\
{\
    LATAN_ERROR(Size, "provided error vector has a wrong size"\
                " (expected " + strFrom((var).rows()) + ", got "\
                + strFrom((err).size()) + ")");\
}

#define checkModelVec(v)\
if (static_cast<Index>((v).size()) != getNYDim())\
{\
    LATAN_ERROR(Size, "provided model vector has a wrong size"\
                " (expected " + strFrom(getNYDim()) + ", got "\
                + strFrom((v).size()) + ")");\
}\
for (unsigned int _i = 1; _i < (v).size(); ++_i)\
{\
    if ((v)[_i]->getNArg() != getNXDim())\
    {\
        LATAN_ERROR(Size, "model " + strFrom(_i) + " has a wrong"\
                    + " number of argument (expected " + strFrom(getNXDim())\
                    + ", got " + strFrom((v)[_i]->getNArg()));\
    }\
}\
{\
    Index _nPar = (v)[0]->getNPar();\
    for (unsigned int _i = 1; _i < (v).size(); ++_i)\
    {\
        if ((v)[_i]->getNPar() != _nPar)\
        {\
            LATAN_ERROR(Size, "model " + strFrom(_i) + " has a wrong"\
                        + " number of parameter (expected " + strFrom(_nPar)\
                        + ", got " + strFrom((v)[_i]->getNPar()));\
        }\
    }\
}

END_LATAN_NAMESPACE

#endif // Latan_XYStatData_hpp_
