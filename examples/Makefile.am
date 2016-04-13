if CXX_GNU
    COM_CXXFLAGS = -Wall -W -pedantic -Wno-deprecated-declarations
else
if CXX_INTEL
    COM_CXXFLAGS = -wd1682 -Wall
endif
endif

noinst_PROGRAMS =           \
    exCompiledDoubleFunction\
    exDerivative            \
    exFit                   \
    exFitSample             \
    exIntegrator            \
    exInterp                \
    exMat                   \
    exMathInterpreter       \
    exMin                   \
    exPlot                  \
    exRand                  \
    exRootFinder

exCompiledDoubleFunction_SOURCES  = exCompiledDoubleFunction.cpp
exCompiledDoubleFunction_CXXFLAGS = $(COM_CXXFLAGS)
exCompiledDoubleFunction_LDFLAGS  = -L../lib/.libs -lLatAnalyze

exDerivative_SOURCES              = exDerivative.cpp
exDerivative_CXXFLAGS             = $(COM_CXXFLAGS)
exDerivative_LDFLAGS              = -L../lib/.libs -lLatAnalyze

exFit_SOURCES                     = exFit.cpp
exFit_CXXFLAGS                    = $(COM_CXXFLAGS)
exFit_LDFLAGS                     = -L../lib/.libs -lLatAnalyze

exFitSample_SOURCES               = exFitSample.cpp
exFitSample_CXXFLAGS              = $(COM_CXXFLAGS)
exFitSample_LDFLAGS               = -L../lib/.libs -lLatAnalyze

exInterp_SOURCES                  = exInterp.cpp
exInterp_CXXFLAGS                 = $(COM_CXXFLAGS)
exInterp_LDFLAGS                  = -L../lib/.libs -lLatAnalyze

exIntegrator_SOURCES              = exIntegrator.cpp
exIntegrator_CXXFLAGS             = $(COM_CXXFLAGS)
exIntegrator_LDFLAGS              = -L../lib/.libs -lLatAnalyze

exMat_SOURCES                     = exMat.cpp
exMat_CXXFLAGS                    = $(COM_CXXFLAGS)
exMat_LDFLAGS                     = -L../lib/.libs -lLatAnalyze

exMin_SOURCES                     = exMin.cpp
exMin_CXXFLAGS                    = $(COM_CXXFLAGS)
exMin_LDFLAGS                     = -L../lib/.libs -lLatAnalyze

exMathInterpreter_SOURCES         = exMathInterpreter.cpp
exMathInterpreter_CXXFLAGS        = $(COM_CXXFLAGS)
exMathInterpreter_LDFLAGS         = -L../lib/.libs -lLatAnalyze

exPlot_SOURCES                    = exPlot.cpp
exPlot_CXXFLAGS                   = $(COM_CXXFLAGS)
exPlot_LDFLAGS                    = -L../lib/.libs -lLatAnalyze

exRand_SOURCES                    = exRand.cpp
exRand_CXXFLAGS                   = $(COM_CXXFLAGS)
exRand_LDFLAGS                    = -L../lib/.libs -lLatAnalyze

exRootFinder_SOURCES              = exRootFinder.cpp
exRootFinder_CXXFLAGS             = $(COM_CXXFLAGS)
exRootFinder_LDFLAGS              = -L../lib/.libs -lLatAnalyze

ACLOCAL_AMFLAGS = -I .buildutils/m4
