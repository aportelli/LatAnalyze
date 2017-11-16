/*
 * FFT.hpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2017 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Latan_FFT_hpp_
#define Latan_FFT_hpp_

#include <LatAnalyze/Global.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                             FFT abstract class                             *
 ******************************************************************************/

class FFT
{
public:
    enum
    {
      Forward  = 0,
      Backward = 1
    };
public:
  // constructor
  FFT(void) = default;
  FFT(const Index size);
  // destructor
  virtual ~FFT(void) = default;
  // size
  virtual void resize(const Index size) = 0;
  // FFT
  virtual void operator()(CMat &x, const unsigned int dir = FFT::Forward) = 0;
};

END_LATAN_NAMESPACE

#endif // Latan_FFT_hpp_
