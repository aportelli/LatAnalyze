#ifndef LATAN_SAMPLE_HPP_
#define	LATAN_SAMPLE_HPP_

#include <latan/Global.hpp>
#include <latan/Mat.hpp>

LATAN_BEGIN_CPPDECL

const int Central = -1;

class Sample
{
private:
    // type alias for disambiguation
    typedef Eigen::Array<DMat,Eigen::Dynamic,1> ArrayType;
    
public:
    // Constructors/destructor
    Sample(void);
    Sample(const unsigned int init_nsample, const unsigned int init_nrow,\
           const unsigned int init_ncol);
    ~Sample(void);
    // Operators
    DMat& operator()(const int s);
    
private:
    // type aliases
    DMat      central;
    ArrayType sample;
};

LATAN_END_CPPDECL

#endif
