/*
 * FitInterface.hpp, part of LatAnalyze 3
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

#ifndef Latan_FitInterface_hpp_
#define Latan_FitInterface_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Mat.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                             FitInterface                                   *
 ******************************************************************************/
class FitInterface
{
private:
    typedef struct
    {
        Index                           nXFitDim, nYFitDim;
        // X/Y block sizes
        Index                           totalSize, totalXSize, totalYSize;
        // size of each X/Y dimension
        std::vector<Index>              xSize, ySize;
        // set of active data indices
        std::set<Index>                 dataIndexSet;
        // lookup tables
        // xDim        : x fit dim ifit -> x dim i
        // x           : x fit point ifit,rfit -> x point r
        // xFitDim     : x dim i -> x fit dim ifit (-1 if empty)
        // xFit        : x point i,r -> x fit point rfit (-1 if empty)
        // data        : y fit point jfit,sfit -> y point index k
        // yFitFromData: y point index k,j -> y fit point sfit (-1 if empty)
        // xIndFromData: data index k -> index of coordinates of associated x
        std::vector<Index>                  xDim, yDim, xFitDim, yFitDim;
        std::vector<std::vector<Index>>     x, y, data, xFit, yFit;
        std::vector<std::map<Index, Index>> yFitFromData;
        std::map<Index, std::vector<Index>> xIndFromData;
    } Layout;
public:
    // constructor
    FitInterface(void);
    // destructor
    virtual ~FitInterface(void) = default;
    // copy object (not as a constructor to be accessed from derived class)
    void copyInterface(const FitInterface &d);
    // add dimensions
    void  addXDim(const Index nData, const std::string name = "",
                  const bool isExact = false);
    void  addYDim(const std::string name = "");
    // access
          Index             getNXDim(void) const;
          Index             getNYDim(void) const;
          Index             getXSize(void) const;
          Index             getXSize(const Index i) const;
          Index             getYSize(void) const;
          Index             getYSize(const Index j) const;
          Index             getXFitSize(void) const;
          Index             getXFitSize(const Index i) const;
          Index             getYFitSize(void) const;
          Index             getYFitSize(const Index j) const;
          Index             getMaxDataIndex(void) const;
    const std::set<Index> & getDataIndexSet(void) const;
          double            getSvdTolerance(void) const;
          void              setSvdTolerance(const double &tol);
          VarName &         xName(void);
    const VarName &         xName(void) const;
          VarName &         yName(void);
    const VarName &         yName(void) const;
    
    // Y dimension index helper
    template <typename... Ts>
    Index                      dataIndex(const Ts... is) const;
    Index                      dataIndex(const std::vector<Index> &v) const;
    const std::vector<Index> & dataCoord(const Index k) const;
    // enable fit points
    void fitPoint(const bool isFitPoint, const Index k, const Index j = 0);
    // variance interface
    void assumeXExact(const bool isExact, const Index i);
    void assumeXXCorrelated(const bool isCorr, const Index r1, const Index i1,
                            const Index r2, const Index i2);
    void assumeXXCorrelated(const bool isCorr, const Index i1, const Index i2);
    void assumeYYCorrelated(const bool isCorr, const Index k1, const Index j1,
                            const Index k2, const Index j2);
    void assumeYYCorrelated(const bool isCorr, const Index j1, const Index j2);
    void assumeXYCorrelated(const bool isCorr, const Index r, const Index i,
                            const Index k, const Index j);
    void assumeXYCorrelated(const bool isCorr, const Index i, const Index j);
    // tests
    bool pointExists(const Index k) const;
    bool pointExists(const Index k, const Index j) const;
    bool isXExact(const Index i) const;
    bool isXUsed(const Index r, const Index i, const bool inFit = true) const;
    bool isFitPoint(const Index k, const Index j) const;
    // make correlation filter for fit variance matrix
    DMat makeCorrFilter(void);
    // schedule variance matrix initialization
    void  scheduleFitVarMatInit(const bool init = true);
    // IO
    friend std::ostream & operator<<(std::ostream &out, FitInterface &f);
protected:
    // register a data point
    void registerDataPoint(const Index k, const Index j = 0);
    // add correlation to a set
    static void addCorr(std::set<std::array<Index, 4>> &s, const bool isCorr,
                        const std::array<Index, 4> &c);
    // abstract methods to create data containers
    virtual void createXData(const std::string name, const Index nData) = 0;
    virtual void createYData(const std::string name) = 0;
    // coordinate buffering
    void scheduleDataCoordInit(void);
    void updateDataCoord(void) const;
    // global layout management
    void  scheduleLayoutInit(void);
    bool  initVarMat(void) const;
    void  updateLayout(void) const;
    Index indX(const Index r, const Index i) const;
    Index indY(const Index k, const Index j) const;
private:
    // function to convert an row-major index into coordinates
    std::vector<Index> rowMajToCoord(const Index k) const;
protected:
    Layout layout;
private:
    VarName                             xName_, yName_;
    std::vector<Index>                  xSize_;
    std::vector<bool>                   xIsExact_;
    std::map<Index, std::vector<Index>> dataCoord_;
    std::set<Index>                     dataIndexSet_;
    std::vector<std::map<Index, bool>>  yDataIndex_;
    std::set<std::array<Index, 4>>      xxCorr_, yyCorr_, xyCorr_;
    Index                               maxDataIndex_{1};
    bool                                initLayout_{true};
    bool                                initVarMat_{true};
    bool                                initDataCoord_{true};
    double                              svdTol_{1.e-10};
};

std::ostream & operator<<(std::ostream &out, FitInterface &f);

/******************************************************************************
 *                  FitInterface template implementation                      *
 ******************************************************************************/
// Y dimension index helper ////////////////////////////////////////////////////
template <typename... Ts>
Index FitInterface::dataIndex(const Ts... coords) const
{
    static_assert(static_or<std::is_convertible<Index, Ts>::value...>::value,
                  "fitPoint arguments are not compatible with Index");
    
    const std::vector<Index> coord = {coords...};

    return dataIndex(coord);
}

/******************************************************************************
 *                       error check macros                                   *
 ******************************************************************************/
#define checkXDim(i)\
if ((i) >= getNXDim())\
{\
    LATAN_ERROR(Range, "X dimension " + strFrom(i) + " out of range");\
}

#define checkXIndex(vi, i)\
if ((vi) >= getXSize(i))\
{\
    LATAN_ERROR(Range, "index " + strFrom(vi) + " in X dimension "\
                + strFrom(i) + " out of range");\
}

#define checkYDim(j)\
if ((j) >= getNYDim())\
{\
    LATAN_ERROR(Range, "Y dimension " + strFrom(j) + " out of range");\
}

#define checkDataIndex(k)\
if ((k) >= getMaxDataIndex())\
{\
    LATAN_ERROR(Range, "data point index " + strFrom(k) + " invalid");\
}

#define checkDataCoord(v)\
if (static_cast<Index>((v).size()) != getNXDim())\
{\
    LATAN_ERROR(Size, "number of coordinates and number of X dimensions "\
                "mismatch");\
}\
for (unsigned int i_ = 0; i_ < (v).size(); ++i_)\
{\
    checkXIndex((v)[i_], i_);\
}

#define checkPoint(k, j)\
if (!pointExists(k, j))\
{\
    LATAN_ERROR(Range, "no data point in Y dimension " + strFrom(j)\
                + " with index " + strFrom(k));\
}

END_LATAN_NAMESPACE

#endif // Latan_FitInterface_hpp_
