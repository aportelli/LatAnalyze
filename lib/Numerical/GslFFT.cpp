/*
 * GslFFT.cpp, part of LatAnalyze
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#include <LatAnalyze/Numerical/GslFFT.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                           GslFFT implementation                            *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
GslFFT::GslFFT(const Index size)
{
    resize(size);
}

// destructor //////////////////////////////////////////////////////////////////
GslFFT::~GslFFT(void)
{
    clear();
}

// size ////////////////////////////////////////////////////////////////////////
void GslFFT::resize(const Index size)
{
    if (size_ != size)
    {
        size_      = size;
        wavetable_ = gsl_fft_complex_wavetable_alloc(size_);
        workspace_ = gsl_fft_complex_workspace_alloc(size_);
    }
}

// fft /////////////////////////////////////////////////////////////////////////
void GslFFT::operator()(CMat &x, const unsigned int dir)
{
    if (x.size() != size_)
    {
        LATAN_ERROR(Size, "wrong input vector size");
    }
    else
    {
        switch (dir)
        {
            case FFT::Forward:
                gsl_fft_complex_forward((double *)x.data(), 1, size_,
                                        wavetable_, workspace_);
                break;
            case FFT::Backward:
                gsl_fft_complex_backward((double *)x.data(), 1, size_,
                                         wavetable_, workspace_);
                break;
            default:
                LATAN_ERROR(Argument, "invalid FT direction");
                break;
        }
    }
}

// destroy GSL objects /////////////////////////////////////////////////////////
void GslFFT::clear(void)
{
    if (!wavetable_)
    {
        gsl_fft_complex_wavetable_free(wavetable_);
    }
    if (!workspace_)
    {
        gsl_fft_complex_workspace_free(workspace_);
    }
}
