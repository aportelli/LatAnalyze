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
    exCompiledDoubleFunction\
    exMat                   \
    exMathInterpreter       \
    exMin                   \
    exPlot                  \
    exRand

exCompiledDoubleFunction_SOURCES = exCompiledDoubleFunction.cpp
exCompiledDoubleFunction_CFLAGS  = -g -O2
exCompiledDoubleFunction_LDFLAGS = -L../latan/.libs -llatan

exMat_SOURCES                    = exMat.cpp
exMat_CFLAGS                     = -g -O2
exMat_LDFLAGS                    = -L../latan/.libs -llatan

exMin_SOURCES                    = exMin.cpp
exMin_CFLAGS                     = -g -O2
exMin_LDFLAGS                    = -L../latan/.libs -llatan

exMathInterpreter_SOURCES        = exMathInterpreter.cpp
exMathInterpreter_CFLAGS         = -g -O2
exMathInterpreter_LDFLAGS        = -L../latan/.libs -llatan

exPlot_SOURCES                   = exPlot.cpp
exPlot_CFLAGS                    = -g -O2
exPlot_LDFLAGS                   = -L../latan/.libs -llatan

exRand_SOURCES                   = exRand.cpp
exRand_CFLAGS                    = -g -O2
exRand_LDFLAGS                   = -L../latan/.libs -llatan

ACLOCAL_AMFLAGS = -I .buildutils/m4
