#include <latan/Sample.hpp>
#include <latan/includes.hpp>

using namespace Latan;

Sample::Sample(void)
: central(0,0), sample(static_cast<ArrayType::Index>(0))
{}

Sample::Sample(const unsigned int init_nsample, const unsigned int init_nrow,\
               const unsigned int init_ncol)
: central(init_nrow,init_ncol), sample(init_nsample)
{
    for (unsigned int s=0;s<init_nsample;++s)
    {
        sample(s).resize(init_nrow,init_ncol);
    }
}

DMat& Sample::operator()(const int s)
{
    if (s >= 0)
    {
        return sample(s);
    }
    else
    {
        return central;
    }
}
