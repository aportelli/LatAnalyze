#ifndef LATAN_SAMPLE_H_
#define	LATAN_SAMPLE_H_

#include <latan/Global.h>

LATAN_BEGIN_CPPDECL

const int Central = -1;

class Sample
{
public:
    // Constructors/destructor
    Sample(void);
    Sample(Sample &S);
    Sample(const unsigned int init_nsample, const unsigned int init_nrow,\
           const unsigned int init_ncol);
    ~Sample(void);
    // Operators
    DMat& operator()(const int s);
    
private:
    DMat central;
    Eigen::Array<DMat,Eigen::Dynamic,1> sample;
};

LATAN_END_CPPDECL

#endif
