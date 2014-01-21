#include <latan/Mat.hpp>
#include <latan/includes.hpp>

using namespace std;
using namespace Latan;

/******************************************************************************
 *                                 DMat class                                 *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
DMat::DMat(void)
: DMatBase()
{}

DMat::DMat(const DMat& M)
: DMatBase(M)
{}

DMat::DMat(unsigned int nrow, unsigned int ncol)
: DMatBase(nrow,ncol)
{}

IOObject::IOType DMat::getType(void)
{
    return dMat;
}
