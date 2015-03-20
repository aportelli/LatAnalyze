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
    exDerivative            \
    exIntegrator            \
    exMat                   \
    exMathInterpreter       \
    exPlot                  \
    exRand                  \
    exRootFinder

if HAVE_MINUIT
noinst_PROGRAMS += exFit exMin
endif

exCompiledDoubleFunction_SOURCES = exCompiledDoubleFunction.cpp
exCompiledDoubleFunction_CFLAGS  = -g -O2
exCompiledDoubleFunction_LDFLAGS = -L../lib/.libs -lLatAnalyze

exDerivative_SOURCES             = exDerivative.cpp
exDerivative_CFLAGS              = -g -O2
exDerivative_LDFLAGS             = -L../lib/.libs -lLatAnalyze

if HAVE_MINUIT
exFit_SOURCES                    = exFit.cpp
exFit_CFLAGS                     = -g -O2
exFit_LDFLAGS                    = -L../lib/.libs -lLatAnalyze
endif

exIntegrator_SOURCES             = exIntegrator.cpp
exIntegrator_CFLAGS              = -g -O2
exIntegrator_LDFLAGS             = -L../lib/.libs -lLatAnalyze

exMat_SOURCES                    = exMat.cpp
exMat_CFLAGS                     = -g -O2
exMat_LDFLAGS                    = -L../lib/.libs -lLatAnalyze

if HAVE_MINUIT
exMin_SOURCES                    = exMin.cpp
exMin_CFLAGS                     = -g -O2
exMin_LDFLAGS                    = -L../lib/.libs -lLatAnalyze
endif

exMathInterpreter_SOURCES        = exMathInterpreter.cpp
exMathInterpreter_CFLAGS         = -g -O2
exMathInterpreter_LDFLAGS        = -L../lib/.libs -lLatAnalyze

exPlot_SOURCES                   = exPlot.cpp
exPlot_CFLAGS                    = -g -O2
exPlot_LDFLAGS                   = -L../lib/.libs -lLatAnalyze

exRand_SOURCES                   = exRand.cpp
exRand_CFLAGS                    = -g -O2
exRand_LDFLAGS                   = -L../lib/.libs -lLatAnalyze

exRootFinder_SOURCES             = exRootFinder.cpp
exRootFinder_CFLAGS              = -g -O2
exRootFinder_LDFLAGS             = -L../lib/.libs -lLatAnalyze

ACLOCAL_AMFLAGS = -I .buildutils/m4
