/*
 * GslFFT.hpp, part of LatAnalyze
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

#ifndef Latan_GslFFT_hpp_
#define Latan_GslFFT_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Mat.hpp>
#include <LatAnalyze/FFT.hpp>
#include <gsl/gsl_fft_complex.h>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                                 GSL FFT                                    *
 ******************************************************************************/

class GslFFT: public FFT
{
public:
    // constructors
    GslFFT(void) = default;
    GslFFT(const Index size);
    // destructor
    virtual ~GslFFT(void);
    // size
    void resize(const Index size);
    // fft
    virtual void operator()(CMat &x, const unsigned int dir = FFT::Forward);
private:
    // destroy GSL objects
    void clear(void);
private:
    Index                     size_{0};
    gsl_fft_complex_wavetable *wavetable_{nullptr};
    gsl_fft_complex_workspace *workspace_{nullptr};
};

END_LATAN_NAMESPACE

#endif // Latan_GslFFT_hpp_
