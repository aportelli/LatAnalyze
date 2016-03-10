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

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                             FitInterface                                   *
 ******************************************************************************/
class FitInterface
{
public:
    // constructor
    FitInterface(void);
    // destructor
    virtual ~FitInterface(void) = default;
    // add dimensions
    void  addXDim(const std::string name, const Index nData);
    void  addYDim(const std::string name);
    // size access
    Index getNXDim(void) const;
    Index getNYDim(void) const;
    Index getXSize(void) const;
    Index getXSize(const Index i) const;
    Index getYSize(void) const;
    Index getYSize(const Index j) const;
    Index getXFitSize(void) const;
    Index getXFitSize(const Index i) const;
    Index getYFitSize(void) const;
    Index getYFitSize(const Index j) const;
    // Y dimension index helper
    template <typename... Ts>
    Index              dataIndex(const Ts... is) const;
    Index              dataIndex(const std::vector<Index> &v) const;
    std::vector<Index> dataCoord(const Index k) const;
    // enable fit points
    void fitPoint(const bool isFitPoint, const Index k, const Index j = 0);
    // tests
    bool isXUsed(const Index k) const;
    bool isXUsed(const Index k, const Index j) const;
    bool isFitPoint(const Index k, const Index j) const;
    // IO
    friend std::ostream & operator<<(std::ostream &out, FitInterface &f);
protected:
public:
    // register a data point
    void registerDataPoint(const Index k, const Index j = 0);
    // abstract method to update data container size
    virtual void updateDataSize(void) {};
private:
    std::vector<std::string>           xDimName_, yDimName_;
    std::map<std::string, Index>       xDimIndex_, yDimIndex_;
    std::vector<Index>                 xSize_;
    std::vector<std::map<Index, bool>> yDataIndex_;
    Index                              maxDataIndex_{1};
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

END_LATAN_NAMESPACE

#endif // Latan_FitInterface_hpp_
