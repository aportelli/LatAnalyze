if CC_GNU
    COM_CFLAGS = -Wall -W -pedantic
else
if CC_INTEL
    COM_CFLAGS = -Wall
endif
endif

if CXX_GNU
    COM_CXXFLAGS = -Wall -W -pedantic
else
if CXX_INTEL
    COM_CXXFLAGS = -Wall
endif
endif

noinst_PROGRAMS =           \
    exMathInterpreter          \
    exCompiledDoubleFunction

exMathInterpreter_SOURCES        = exMathInterpreter.cpp
exMathInterpreter_CFLAGS         = -g -O2
exMathInterpreter_LDFLAGS        = -L../latan/.libs -llatan

exCompiledDoubleFunction_SOURCES = exCompiledDoubleFunction.cpp
exCompiledDoubleFunction_CFLAGS  = -g -O2
exCompiledDoubleFunction_LDFLAGS = -L../latan/.libs -llatan

ACLOCAL_AMFLAGS = -I .buildutils/m4
