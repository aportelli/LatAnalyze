#include <latan/Exceptions.hpp>
#include <latan/includes.hpp>

#ifndef ERR_PREF
#define ERR_PREF "[" + Env::name + " v" + Env::version + "] "
#endif
#ifndef ERR_SUFF
#define ERR_SUFF " (" + loc + ")"
#endif

#define CONST_EXC(name,init) \
name::name(string msg, string loc)\
:init\
{}

using namespace std;
using namespace Latan;
using namespace Exceptions;

// logic errors
CONST_EXC(Logic,logic_error(ERR_PREF+msg+ERR_SUFF))
CONST_EXC(Implementation,Logic("implementation error: "+msg,loc))
CONST_EXC(Range,Logic("range error: "+msg,loc))
// runtime errors
CONST_EXC(Runtime,runtime_error(ERR_PREF+msg+ERR_SUFF))
CONST_EXC(Compilation,Runtime("compilation error: "+msg,loc))
CONST_EXC(Io,Runtime("IO error: "+msg,loc))
CONST_EXC(Parsing,Runtime(msg,loc))
CONST_EXC(Syntax,Runtime("syntax error: "+msg,loc))

