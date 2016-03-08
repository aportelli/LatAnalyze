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
    void         addXDim(const std::string name, const Index nData);
    void         addYDim(const std::string name);
    unsigned int getNXDim(void);
    unsigned int getNYDim(void);
    // Y dimension index helper
    template <typename... Ts>
    Index yIndex(const Ts... is);
    // enable fit points
    void fitPoint(const bool isFitPoint, const Index j);
private:
    void updateYSize(void);
private:
    std::vector<std::string>     xDimName_, yDimName_;
    std::map<std::string, Index> xIndex_, yIndex_;
    std::vector<Index>           xSize_;
    Index                        ySize_;
public:
    IVec                         isFitPoint_;
};

/******************************************************************************
 *                  FitInterface template implementation                      *
 ******************************************************************************/
// Y dimension index helper ////////////////////////////////////////////////////
template <typename... Ts>
Index FitInterface::yIndex(const Ts... is)
{
    static_assert(static_or<std::is_convertible<Index, Ts>::value...>::value,
                  "fitPoint arguments are not compatible with Index");
    
    constexpr size_t n = sizeof...(is);
    
    if (n != getNXDim())
    {
        LATAN_ERROR(Size, "number of arguments and number of X dimensions "
                          "mismatch");
    }
    
    constexpr std::array<Index, sizeof...(is)> i = {is...};
    Index                                      j;
    
    for (unsigned int d = 0; d < n; ++d)
    {
        if (i[d] >= xSize_[d])
        {
            LATAN_ERROR(Range, "index in X dimension " + strFrom(d)
                               + " out of range");
        }
    }
    j = xSize_[1]*i[0];
    for (unsigned int d = 1; d < n-1; ++d)
    {
        j = xSize_[d+1]*(i[d] + j);
    }
    j += i[n-1];

    return j;
}

END_LATAN_NAMESPACE

#endif // Latan_FitInterface_hpp_
