/*
 * Random.cpp, part of LatAnalyze 3
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

#include <LatAnalyze/Core/Plot.hpp>
#include <LatAnalyze/includes.hpp>
#include <LatAnalyze/Statistics/Random.hpp>

using namespace std;
using namespace Latan;

RandomNormal::RandomNormal(const DVec &mean, const DMat &var, const SeedType seed)
: mean_(mean), buf_(mean.size()), var_(var), gen_(seed)
{
  if (var_.rows() != var_.cols())
  {
    LATAN_ERROR(Size, "variance matrix not square");
  }
  if (mean_.size() != var_.rows())
  {
    LATAN_ERROR(Size, "variance matrix and mean vector size mismatch");
  }
  
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> esolver(var);
  Eigen::VectorXd                                ev = esolver.eigenvalues();

  ev = ev.unaryExpr([](const double x){return (x > 0.) ? x : 0.;});
  transform_ = esolver.eigenvectors()*ev.cwiseSqrt().asDiagonal();
}

DVec RandomNormal::operator()(void)
{
  std::normal_distribution<> dist;

  FOR_VEC(buf_, i)
  {
    buf_(i) = dist(gen_);
  }

  return mean_ + transform_*buf_;
}
