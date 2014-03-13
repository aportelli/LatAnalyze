/*
 * RandGen.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2014 Antonin Portelli
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

#ifndef Latan_RandGen_hpp_
#define Latan_RandGen_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/IoObject.hpp>

#define RLXG_STATE_SIZE 105u

BEGIN_NAMESPACE

class RandGenState: public Array<int, RLXG_STATE_SIZE, 1>,
                    public IoObject
{
private:
    typedef Array<int, RLXG_STATE_SIZE, 1> Base;
public:
    // destructor
    virtual ~RandGenState(void) = default;
    // IO type
    IoType getType(void) const;
};

/******************************************************************************
 *                          Random generator class                            *
 ******************************************************************************/

class RandGen
{
private:
    // Martin Luescher's ranlxd generator interface
    class RanLxd
    {
    private:
        typedef struct alignas(16)
        {
            float c1,c2,c3,c4;
        } rlxd_vec_t;
        typedef struct alignas(16)
        {
            rlxd_vec_t c1,c2;
        } rlxd_dble_vec_t;
    public:
        RanLxd(void);
        ~RanLxd(void) = default;
        void ranlxd(double r[],int n);
        void rlxd_init(int level,int seed);
        int  rlxd_size(void) const;
        void rlxd_get(int state[]) const;
        void rlxd_reset(const int state[]);
    private:
        void error(int no) const;
        void update(void);
        void define_constants(void);
    private:
        int        init{0}, rlxd_pr, prm, ir, jr, is, is_old, next[96];
        rlxd_vec_t one_sse, one_bit_sse, carry;
        double     one_bit;
        union alignas(16)
        {
            rlxd_dble_vec_t vec[12];
            float num[96];
        } rlxd_x;
    };
public:
    // constructors
    RandGen(void);
    explicit RandGen(const int seed);
    explicit RandGen(const RandGenState &state);
    // destructor
    virtual ~RandGen(void) = default;
    // state management
    RandGenState getState(void) const;
    void         setState(const RandGenState &state);
    // generators
    double uniform(const double a = 0.0, const double b = 1.0);
    double discreteUniform(const unsigned int n);
    double gaussian(const double mean = 0.0, const double sigma = 1.0);
private:
    RanLxd generator_;
};

END_NAMESPACE

#endif // Latan_RandGen_hpp_
