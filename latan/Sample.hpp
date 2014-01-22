#ifndef LATAN_SAMPLE_HPP_
#define	LATAN_SAMPLE_HPP_

#include <latan/Global.hpp>
#include <latan/Mat.hpp>

LATAN_BEGIN_CPPDECL

const int Central = -1;

typedef Eigen::Array<DMat, Eigen::Dynamic, 1> DSampleBase;

class DSample: public DSampleBase
{
public:
    // Constructors/destructor
    DSample(void);
    DSample(const unsigned int nSample, const unsigned int nRow,
            const unsigned int nCol);
    ~DSample(void);
    // Operators
    DMat& operator()(const int s);
};

LATAN_END_CPPDECL

#endif
