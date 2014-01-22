#include <latan/Sample.hpp>
#include <latan/includes.hpp>

using namespace Latan;

DSample::DSample(void)
: DSampleBase(static_cast<Index>(0))
{}

DSample::DSample(const unsigned int nSample, const unsigned int nRow,
                 const unsigned int nCol)
: DSampleBase(static_cast<Index>(nSample + 1))
{
    for (int s = 0; s < size(); ++s)
    {
        (*this)(s).resize(nRow, nCol);
    }
}

DMat& DSample::operator()(const int s)
{
    if (s >= 0)
    {
        return (*this)(s + 1);
    }
    else
    {
        return (*this)(0);
    }
}
