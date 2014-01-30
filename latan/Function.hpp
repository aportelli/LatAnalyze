#ifndef LATAN_FUNCTION_HPP_
#define	LATAN_FUNCTION_HPP_

#include <latan/Global.hpp>
#include <stack>
#include <vector>
#include <cstdarg>

LATAN_BEGIN_CPPDECL

/******************************************************************************
 *                            Base function class                             *
 ******************************************************************************/
class Function
{
public:
    // constructor/destructor
    explicit Function(const unsigned nArg);
    ~Function(void);
    // access
    unsigned int getNArg(void);
private:
    const unsigned int nArg_;
};

/******************************************************************************
 *                            Double function class                           *
 ******************************************************************************/
class DoubleFunction: public Function
{
public:
    // constructor/destructor
    explicit DoubleFunction(const unsigned nArg);
    virtual ~DoubleFunction(void);
    // function call
    virtual double operator()(std::vector<double> &arg) = 0;
    double operator()(std::stack<double> &arg);
    double operator()(const double x0, ...);
private:
    std::vector<double> buffer_;
};

LATAN_END_CPPDECL

#endif
