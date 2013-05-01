#include <latan/Sample.h>

using namespace LATAN_NAMESPACE_NAME;

Sample::Sample(void)
: central(0,0), sample(0)
{}

Sample::Sample(const unsigned int init_nsample, const unsigned int init_nrow,\
               const unsigned int init_ncol)
: central(init_nrow,init_ncol), sample(init_nsample)
{
    for (int s=0;s<init_nsample;++s)
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
