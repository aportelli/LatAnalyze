/*
 * RandGen.cpp, part of LatAnalyze 3
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

#include <latan/RandGen.hpp>
#include <latan/includes.hpp>

#ifndef RLXD_LEVEL
#define RLXD_LEVEL 1
#endif

using namespace std;
using namespace Latan;

/******************************************************************************
 *                        RandGen implementation                              *
 ******************************************************************************/
// State implentation //////////////////////////////////////////////////////////
RandGen::State::State(void)
{}

RandGen::State::~State(void)
{}

unsigned int RandGen::State::getType(void) const
{
    return IoType::rgState;
}

// RanLxd implementation ///////////////////////////////////////////////////////
RandGen::RanLxd::RanLxd(void)
: init(0)
{
    // avoid a warning in the SSE case
    one_bit = 0.0;
}

/****************************************************************************/
/* Copyright (C) 2005 Martin Luescher (GPL)
 * This software is distributed under the terms of the GNU General Public
 * License (GPL)
 *
 * Random number generator "ranlxd". See the notes
 *
 *   "User's guide for ranlxs and ranlxd v3.2" (December 2005)
 *
 *   "Algorithms used in ranlux v3.0" (May 2001)
 *
 * for a detailed description
 *
 * The functions are
 *
 *   void ranlxd(double r[],int n)
 *     Computes the next n double-precision random numbers and
 *     assigns them to the elements r[0],...,r[n-1] of the array r[]
 *
 *   void rlxd_init(int level,int seed)
 *     Initialization of the generator
 *
 *   int rlxd_size(void)
 *     Returns the number of integers required to save the state of
 *     the generator
 *
 *   void rlxd_get(int state[])
 *     Extracts the current state of the generator and stores the
 *     information in the array state[N] where N>=rlxd_size()
 *
 *   void rlxd_reset(int state[])
 *     Resets the generator to the state defined by the array state[N]
 */

#ifdef HAVE_SSE

#define STEP(pi,pj)                                     \
__asm__ __volatile__ ("movaps %4, %%xmm4 \n\t"          \
                      "movaps %%xmm2, %%xmm3 \n\t"      \
                      "subps %2, %%xmm4 \n\t"           \
                      "movaps %%xmm1, %%xmm5 \n\t"      \
                      "cmpps $0x6, %%xmm4, %%xmm2 \n\t" \
                      "andps %%xmm2, %%xmm5 \n\t"       \
                      "subps %%xmm3, %%xmm4 \n\t"       \
                      "andps %%xmm0, %%xmm2 \n\t"       \
                      "addps %%xmm4, %%xmm5 \n\t"       \
                      "movaps %%xmm5, %0 \n\t"          \
                      "movaps %5, %%xmm6 \n\t"          \
                      "movaps %%xmm2, %%xmm3 \n\t"      \
                      "subps %3, %%xmm6 \n\t"           \
                      "movaps %%xmm1, %%xmm7 \n\t"      \
                      "cmpps $0x6, %%xmm6, %%xmm2 \n\t" \
                      "andps %%xmm2, %%xmm7 \n\t"       \
                      "subps %%xmm3, %%xmm6 \n\t"       \
                      "andps %%xmm0, %%xmm2 \n\t"       \
                      "addps %%xmm6, %%xmm7 \n\t"       \
                      "movaps %%xmm7, %1"               \
                      :                                 \
                      "=m" ((*pi).c1),                  \
                      "=m" ((*pi).c2)                   \
                      :                                 \
                      "m" ((*pi).c1),                   \
                      "m" ((*pi).c2),                   \
                      "m" ((*pj).c1),                   \
                      "m" ((*pj).c2)                    \
                      :                                 \
                      "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7")

void RandGen::RanLxd::error(int no) const
{
    switch(no)
    {
        case 1:
            LATAN_ERROR(Range, "Bad choice of luxury level (should be 1 or 2)");
            break;
        case 2:
            LATAN_ERROR(Range, "Bad choice of seed (should be between 1 and 2^31-1)");
            break;
        case 3:
            LATAN_ERROR(Runtime, "Undefined state (ranlxd is not initialized)");
            break;
        case 5:
            LATAN_ERROR(Logic, "Unexpected input data");
            break;
    }
}

void RandGen::RanLxd::update(void)
{
    int k,kmax;
    rlxd_dble_vec_t *pmin,*pmax,*pi,*pj;
    
    kmax=rlxd_pr;
    pmin=&rlxd_x.vec[0];
    pmax=pmin+12;
    pi=&rlxd_x.vec[ir];
    pj=&rlxd_x.vec[jr];
    
    __asm__ __volatile__ ("movaps %0, %%xmm0 \n\t"
                          "movaps %1, %%xmm1 \n\t"
                          "movaps %2, %%xmm2"
                          :
                          :
                          "m" (one_bit_sse),
                          "m" (one_sse),
                          "m" (carry)
                          :
                          "xmm0", "xmm1", "xmm2");
    
    for (k=0;k<kmax;k++)
    {
        STEP(pi,pj);
        pi+=1;
        pj+=1;
        if (pi==pmax)
            pi=pmin;
        if (pj==pmax)
            pj=pmin;
    }
    
    __asm__ __volatile__ ("movaps %%xmm2, %0"
                          :
                          "=m" (carry));
    
    ir+=prm;
    jr+=prm;
    if (ir>=12)
        ir-=12;
    if (jr>=12)
        jr-=12;
    is=8*ir;
    is_old=is;
}

void RandGen::RanLxd::define_constants(void)
{
    int k;
    float b;
    
    one_sse.c1=1.0f;
    one_sse.c2=1.0f;
    one_sse.c3=1.0f;
    one_sse.c4=1.0f;
    
    b=(float)(ldexp(1.0,-24));
    one_bit_sse.c1=b;
    one_bit_sse.c2=b;
    one_bit_sse.c3=b;
    one_bit_sse.c4=b;
    
    for (k=0;k<96;k++)
    {
        next[k]=(k+1)%96;
        if ((k%4)==3)
            next[k]=(k+5)%96;
    }
}

void RandGen::RanLxd::rlxd_init(int level,int seed)
{
    int i,k,l;
    int ibit,jbit,xbit[31];
    int ix,iy;
    
    define_constants();
    
    if (level==1)
        rlxd_pr=202;
    else if (level==2)
        rlxd_pr=397;
    else
        error(1);
    
    i=seed;
    
    for (k=0;k<31;k++)
    {
        xbit[k]=i%2;
        i/=2;
    }
    
    if ((seed<=0)||(i!=0))
        error(2);
    
    ibit=0;
    jbit=18;
    
    for (i=0;i<4;i++)
    {
        for (k=0;k<24;k++)
        {
            ix=0;
            
            for (l=0;l<24;l++)
            {
                iy=xbit[ibit];
                ix=2*ix+iy;
                
                xbit[ibit]=(xbit[ibit]+xbit[jbit])%2;
                ibit=(ibit+1)%31;
                jbit=(jbit+1)%31;
            }
            
            if ((k%4)!=i)
                ix=16777215-ix;
            
            rlxd_x.num[4*k+i]=(float)(ldexp((double)(ix),-24));
        }
    }
    
    carry.c1=0.0f;
    carry.c2=0.0f;
    carry.c3=0.0f;
    carry.c4=0.0f;
    
    ir=0;
    jr=7;
    is=91;
    is_old=0;
    prm=rlxd_pr%12;
    init=1;
}

void RandGen::RanLxd::ranlxd(double r[],int n)
{
    int k;
    
    if (init==0)
        rlxd_init(1,1);
    
    for (k=0;k<n;k++)
    {
        is=next[is];
        if (is==is_old)
            update();
        r[k]=(double)(rlxd_x.num[is+4])+(double)(one_bit_sse.c1*rlxd_x.num[is]);
    }
}

int RandGen::RanLxd::rlxd_size(void) const
{
    return(RLXG_STATE_SIZE);
}

void RandGen::RanLxd::rlxd_get(int state[]) const
{
    int k;
    float base;
    
    if (init==0)
        error(3);
    
    base=(float)(ldexp(1.0,24));
    state[0]=rlxd_size();
    
    for (k=0;k<96;k++)
        state[k+1]=(int)(base*rlxd_x.num[k]);
    
    state[97]=(int)(base*carry.c1);
    state[98]=(int)(base*carry.c2);
    state[99]=(int)(base*carry.c3);
    state[100]=(int)(base*carry.c4);
    
    state[101]=rlxd_pr;
    state[102]=ir;
    state[103]=jr;
    state[104]=is;
}

void RandGen::RanLxd::rlxd_reset(const int state[])
{
    int k;
    
    define_constants();
    
    if (state[0]!=rlxd_size())
        error(5);
    
    for (k=0;k<96;k++)
    {
        if ((state[k+1]<0)||(state[k+1]>=167777216))
            error(5);
        
        rlxd_x.num[k]=(float)(ldexp((double)(state[k+1]),-24));
    }
    
    if (((state[97]!=0)&&(state[97]!=1))||
        ((state[98]!=0)&&(state[98]!=1))||
        ((state[99]!=0)&&(state[99]!=1))||
        ((state[100]!=0)&&(state[100]!=1)))
        error(5);
    
    carry.c1=(float)(ldexp((double)(state[97]),-24));
    carry.c2=(float)(ldexp((double)(state[98]),-24));
    carry.c3=(float)(ldexp((double)(state[99]),-24));
    carry.c4=(float)(ldexp((double)(state[100]),-24));
    
    rlxd_pr=state[101];
    ir=state[102];
    jr=state[103];
    is=state[104];
    is_old=8*ir;
    prm=rlxd_pr%12;
    init=1;
    
    if (((rlxd_pr!=202)&&(rlxd_pr!=397))||
        (ir<0)||(ir>11)||(jr<0)||(jr>11)||(jr!=((ir+7)%12))||
        (is<0)||(is>91))
        error(5);
}

#else

#define BASE 0x1000000
#define MASK 0xffffff

#define STEP(pi,pj) \
d=(*pj).c1.c1-(*pi).c1.c1-carry.c1; \
(*pi).c2.c1+=(d<0); \
d+=BASE; \
(*pi).c1.c1=d&MASK; \
d=(*pj).c1.c2-(*pi).c1.c2-carry.c2; \
(*pi).c2.c2+=(d<0); \
d+=BASE; \
(*pi).c1.c2=d&MASK; \
d=(*pj).c1.c3-(*pi).c1.c3-carry.c3; \
(*pi).c2.c3+=(d<0); \
d+=BASE; \
(*pi).c1.c3=d&MASK; \
d=(*pj).c1.c4-(*pi).c1.c4-carry.c4; \
(*pi).c2.c4+=(d<0); \
d+=BASE; \
(*pi).c1.c4=d&MASK; \
d=(*pj).c2.c1-(*pi).c2.c1; \
carry.c1=(d<0); \
d+=BASE; \
(*pi).c2.c1=d&MASK; \
d=(*pj).c2.c2-(*pi).c2.c2; \
carry.c2=(d<0); \
d+=BASE; \
(*pi).c2.c2=d&MASK; \
d=(*pj).c2.c3-(*pi).c2.c3; \
carry.c3=(d<0); \
d+=BASE; \
(*pi).c2.c3=d&MASK; \
d=(*pj).c2.c4-(*pi).c2.c4; \
carry.c4=(d<0); \
d+=BASE; \
(*pi).c2.c4=d&MASK

void RandGen::RanLxd::error(int no) const
{
    switch(no)
    {
        case 0:
            LATAN_ERROR(System, "Arithmetic on this machine is not suitable for ranlxd");
            break;
        case 1:
            LATAN_ERROR(Range, "Bad choice of luxury level (should be 1 or 2)");
            break;
        case 2:
            LATAN_ERROR(Range, "Bad choice of seed (should be between 1 and 2^31-1)");
            break;
        case 3:
            LATAN_ERROR(Runtime, "Undefined state (ranlxd is not initialized)");
        case 4:
            LATAN_ERROR(System, "Arithmetic on this machine is not suitable for ranlxd");
            break;
        case 5:
            LATAN_ERROR(Logic, "Unexpected input data");
            break;
    }
}

void RandGen::RanLxd::update(void)
{
    int k,kmax,d;
    rlxd_dble_vec_t *pmin,*pmax,*pi,*pj;
    
    kmax=rlxd_pr;
    pmin=&rlxd_x.vec[0];
    pmax=pmin+12;
    pi=&rlxd_x.vec[ir];
    pj=&rlxd_x.vec[jr];
    
    for (k=0;k<kmax;k++)
    {
        STEP(pi,pj);
        pi+=1;
        pj+=1;
        if (pi==pmax)
            pi=pmin;
        if (pj==pmax)
            pj=pmin;
    }
    
    ir+=prm;
    jr+=prm;
    if (ir>=12)
        ir-=12;
    if (jr>=12)
        jr-=12;
    is=8*ir;
    is_old=is;
}

void RandGen::RanLxd::define_constants(void)
{
    int k;
    
    one_bit=ldexp(1.0,-24);
    
    for (k=0;k<96;k++)
    {
        next[k]=(k+1)%96;
        if ((k%4)==3)
            next[k]=(k+5)%96;
    }
}

void RandGen::RanLxd::rlxd_init(int level,int seed)
{
    int i,k,l;
    int ibit,jbit,xbit[31];
    int ix,iy;
    
    if ((INT_MAX<2147483647)||(FLT_RADIX!=2)||(FLT_MANT_DIG<24)||
        (DBL_MANT_DIG<48))
        error(0);
    
    define_constants();
    
    if (level==1)
        rlxd_pr=202;
    else if (level==2)
        rlxd_pr=397;
    else
        error(1);
    
    i=seed;
    
    for (k=0;k<31;k++)
    {
        xbit[k]=i%2;
        i/=2;
    }
    
    if ((seed<=0)||(i!=0))
        error(2);
    
    ibit=0;
    jbit=18;
    
    for (i=0;i<4;i++)
    {
        for (k=0;k<24;k++)
        {
            ix=0;
            
            for (l=0;l<24;l++)
            {
                iy=xbit[ibit];
                ix=2*ix+iy;
                
                xbit[ibit]=(xbit[ibit]+xbit[jbit])%2;
                ibit=(ibit+1)%31;
                jbit=(jbit+1)%31;
            }
            
            if ((k%4)!=i)
                ix=16777215-ix;
            
            rlxd_x.num[4*k+i]=ix;
        }
    }
    
    carry.c1=0;
    carry.c2=0;
    carry.c3=0;
    carry.c4=0;
    
    ir=0;
    jr=7;
    is=91;
    is_old=0;
    prm=rlxd_pr%12;
    init=1;
}

void RandGen::RanLxd::ranlxd(double r[],int n)
{
    int k;
    
    if (init==0)
        rlxd_init(1,1);
    
    for (k=0;k<n;k++)
    {
        is=next[is];
        if (is==is_old)
            update();
        r[k]=one_bit*((double)(rlxd_x.num[is+4])+one_bit*(double)(rlxd_x.num[is]));
    }
}

int RandGen::RanLxd::rlxd_size(void) const
{
    return(RLXG_STATE_SIZE);
}

void RandGen::RanLxd::rlxd_get(int state[]) const
{
    int k;
    
    if (init==0)
        error(3);
    
    state[0]=rlxd_size();
    
    for (k=0;k<96;k++)
        state[k+1]=rlxd_x.num[k];
    
    state[97]=carry.c1;
    state[98]=carry.c2;
    state[99]=carry.c3;
    state[100]=carry.c4;
    
    state[101]=rlxd_pr;
    state[102]=ir;
    state[103]=jr;
    state[104]=is;
}

void RandGen::RanLxd::rlxd_reset(const int state[])
{
    int k;
    
    if ((INT_MAX<2147483647)||(FLT_RADIX!=2)||(FLT_MANT_DIG<24)||
        (DBL_MANT_DIG<48))
        error(4);
    
    define_constants();
    
    if (state[0]!=rlxd_size())
        error(5);
    
    for (k=0;k<96;k++)
    {
        if ((state[k+1]<0)||(state[k+1]>=167777216))
            error(5);
        
        rlxd_x.num[k]=state[k+1];
    }
    
    if (((state[97]!=0)&&(state[97]!=1))||
        ((state[98]!=0)&&(state[98]!=1))||
        ((state[99]!=0)&&(state[99]!=1))||
        ((state[100]!=0)&&(state[100]!=1)))
        error(5);
    
    carry.c1=state[97];
    carry.c2=state[98];
    carry.c3=state[99];
    carry.c4=state[100];
    
    rlxd_pr=state[101];
    ir=state[102];
    jr=state[103];
    is=state[104];
    is_old=8*ir;
    prm=rlxd_pr%12;
    init=1;
    
    if (((rlxd_pr!=202)&&(rlxd_pr!=397))||
        (ir<0)||(ir>11)||(jr<0)||(jr>11)||(jr!=((ir+7)%12))||
        (is<0)||(is>91))
        error(5);
}

#endif

// constructors ////////////////////////////////////////////////////////////////
RandGen::RandGen(void)
{
    generator_.rlxd_init(RLXD_LEVEL, (int)time(NULL));
}

RandGen::RandGen(const int seed)
{
    generator_.rlxd_init(RLXD_LEVEL, seed);
}

RandGen::RandGen(const State &state)
{
    setState(state);
}

// destructor //////////////////////////////////////////////////////////////////
RandGen::~RandGen(void)
{}

// state management ////////////////////////////////////////////////////////////
RandGen::State RandGen::getState(void) const
{
    State state;
    
    generator_.rlxd_get(state.data());
    
    return state;
}

void RandGen::setState(const State &state)
{
    generator_.rlxd_reset(state.data());
}

// generators //////////////////////////////////////////////////////////////////
double RandGen::uniform(const double a, const double b)
{
    double rx;
    
    generator_.ranlxd(&rx, 1);
    
    return (b-a)*rx + a;
}

double RandGen::discreteUniform(const unsigned int n)
{
    return ((unsigned int)(uniform()*(double)(n)));
}

double RandGen::gaussian(const double mean, const double sigma)
{
    double rx, ry, sqNrm;
    
    do
    {
        rx = uniform(-1.0, 1.0);
        ry = uniform(-1.0, 1.0);
        sqNrm = rx*rx + ry*ry;
    } while ((sqNrm > 1.0)||(sqNrm == 0.0));
    
    return sigma*rx*sqrt(-2.0*log(sqNrm)/sqNrm) + mean;
}
