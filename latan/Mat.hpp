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

class DMat: public DMatBase, public IoObject
{
public:
    // constructors
    DMat(void);
    DMat(const DMat& M);
    DMat(const unsigned int nRow, const unsigned int nCol);
    // IO
    virtual unsigned int getType(void) const;
};

LATAN_END_CPPDECL

#endif
