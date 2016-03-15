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

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                    class for X vs. Y statistical data                      *
 ******************************************************************************/
class XYStatData: public FitInterface
{
public:
    // constructor
    XYStatData(void);
    // destructor
    virtual ~XYStatData(void) = default;
    // schedule fit var matrix update
    void scheduleFitVarMatInit(void);
    // data access
    double &       x(const Index vi, const Index i = 0);
    const double & x(const Index vi, const Index i = 0) const;
    double &       y(const Index k, const Index j = 0);
    const double & y(const Index k, const Index j = 0) const;
    void           setXXVar(const Index i1, const Index i2, const DMat &m);
    void           setYYVar(const Index j1, const Index j2, const DMat &m);
    void           setXYVar(const Index i, const Index j, const DMat &m);
    void           setXError(const Index i, const DVec &err);
    void           setYError(const Index j, const DVec &err);
    const DMat &   getXXVar(const Index i1, const Index i2) const;
    const DMat &   getYYVar(const Index j1, const Index j2) const;
    const DMat &   getXYVar(const Index i, const Index j) const;
    DVec           getXError(const Index i) const;
    DVec           getYError(const Index j) const;
    // get total fit variance matrix
    const DMat &   getFitVar(void);
protected:
    // create data
    virtual void createXData(const Index nData);
    virtual void createYData(void);
    void         resizeVarMat(void);
private:
    // make total fit variance matrix
    void updateFitVarMat(void);
private:
    std::vector<std::map<Index, double>> yData_;
    std::vector<DVec>                    xData_;
    Mat<DMat>                            xxVar_, yyVar_, xyVar_;
    DMat                                 fitVar_;
    bool                                 initVarMat_{true};
};

END_LATAN_NAMESPACE

#endif // Latan_XYStatData_hpp_
