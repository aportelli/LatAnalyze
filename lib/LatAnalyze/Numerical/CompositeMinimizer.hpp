/*
 * CompositeMinimizer.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2024 Antonin Portelli
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

#ifndef Latan_CompositeMinimizer_hpp_
#define Latan_CompositeMinimizer_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Functional/Function.hpp>
#include <LatAnalyze/Numerical/Minimizer.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                       Class for chaining minimizers                        *
 ******************************************************************************/
class CompositeMinimizer: public Minimizer
{
public:
    // constructors
    explicit CompositeMinimizer(const std::vector<Minimizer *> &minimizer);
    template <typename... Ts>
    CompositeMinimizer(Minimizer &min, Ts & ... minimisers);
    // destructor
    virtual ~CompositeMinimizer(void) = default;
    // access
    virtual bool supportLimits(void) const;
    // minimization
    virtual const DVec & operator()(const DoubleFunction &f);
private:
    std::vector<Minimizer *> minVec_;
};

/******************************************************************************
 *              CompositeMinimizer template implementation                    *
 ******************************************************************************/
template <typename... Ts>
CompositeMinimizer::CompositeMinimizer(Minimizer &min, Ts & ... minimisers)
{
    static_assert(static_or<std::is_assignable<Minimizer &, Ts>::value...>::value,
                  "model arguments are not compatible with Minimizer");


    minVec_ = {&min, &minimisers...};
    if (minVec_.size() == 0)
    {
        LATAN_ERROR(Size, "minimizer vector has size 0");
    }
}

END_LATAN_NAMESPACE

#endif // Latan_CompositeMinimizer_hpp_
