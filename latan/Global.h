#ifndef LATAN_GLOBAL_H_
#define	LATAN_GLOBAL_H_

#include <latan/Eigen/Dense>

#define LATAN_NAMESPACE_NAME Latan
#define LATAN_BEGIN_CPPDECL namespace LATAN_NAMESPACE_NAME {
#define LATAN_END_CPPDECL }

LATAN_BEGIN_CPPDECL

typedef Eigen::MatrixXd  DMat;
typedef Eigen::MatrixXcd CMat;
typedef Eigen::VectorXd  DVec;
typedef Eigen::VectorXcd CVec;

LATAN_END_CPPDECL

#endif
