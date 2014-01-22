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

DMat::DMat(const unsigned int nRow, const unsigned int nCol)
: DMatBase(nRow,nCol)
{}

unsigned int DMat::getType(void)
{
    return IoType::dMat;
}
