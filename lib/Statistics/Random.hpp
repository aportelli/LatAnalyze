/*
 * Random.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2019 Antonin Portelli
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

#ifndef Latan_Random_hpp_
#define	Latan_Random_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Core/Mat.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                         Multivariate Gaussian RNG                          *
 ******************************************************************************/
class RandomNormal
{
public:
  // constructors
  RandomNormal(const DVec &mean, const DMat &var, const SeedType seed);
  // destructor
  virtual ~RandomNormal(void) = default;
  // draw a random vector
  DVec operator()(void);
private:
  DVec           mean_, buf_;
  DMat           var_;
  Eigen::MatrixXd transform_;
  std::mt19937 gen_;
};

END_LATAN_NAMESPACE

#endif // Latan_Random_hpp_
