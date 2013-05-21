#ifndef LATAN_MAT_HPP_
#define LATAN_MAT_HPP_

#include <latan/Eigen/Dense>
#include <latan/Global.hpp>
#include <latan/IOObject.hpp>

LATAN_BEGIN_CPPDECL

// Eigen aliases
typedef Eigen::MatrixXd  DMatBase;
typedef Eigen::MatrixXcd CMatBase;
typedef Eigen::VectorXd  DVecBase;
typedef Eigen::VectorXcd CVecBase;

class DMat: public DMatBase, public IOObject
{
public:
    // constructors
    DMat(void);
    DMat(const DMat& M);
    DMat(unsigned int nrow, unsigned int ncol);
    // IO
    virtual IOTypes::Type IOType(void);
};

LATAN_END_CPPDECL

#endif