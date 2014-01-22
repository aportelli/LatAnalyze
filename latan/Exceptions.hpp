#ifndef LATAN_EXCEPTIONS_HPP_
#define LATAN_EXCEPTIONS_HPP_

#include <stdexcept>
#ifndef LATAN_GLOBAL_HPP_
#include <latan/Global.hpp>
#endif

#define SRC_LOC strfrom<const char*>(__FUNCTION__) + " at "\
                + strfrom<const char*>(__FILE__) + ":" + strfrom<int>(__LINE__)
#define LATAN_ERROR(exc,msg) throw(Exceptions::exc(msg,SRC_LOC))

#define DECL_EXC(name,base) \
class name: public base\
{\
public:\
    explicit name(std::string msg, std::string loc);\
}

LATAN_BEGIN_CPPDECL

namespace Exceptions
{
    // logic errors
    DECL_EXC(Logic,std::logic_error);
    DECL_EXC(Implementation,Logic);
    DECL_EXC(Range,Logic);
    // runtime errors
    DECL_EXC(Runtime,std::runtime_error);
    DECL_EXC(Compilation,Runtime);
    DECL_EXC(Io,Runtime);
    DECL_EXC(Parsing,Runtime);
    DECL_EXC(Syntax,Runtime);
}

LATAN_END_CPPDECL

#endif
