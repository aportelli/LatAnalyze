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

noinst_PROGRAMS = \
    exMathCompiler

exMathCompiler_SOURCES = exMathCompiler.cpp
exMathCompiler_CFLAGS  = -g -O2
exMathCompiler_LDFLAGS = -L../latan/.libs -llatan

ACLOCAL_AMFLAGS = -I .buildutils/m4
