COM_CXXFLAGS = -Wall
if CXX_GNU
    COM_CXXFLAGS += -W -pedantic -Wno-deprecated-declarations
else
if CXX_INTEL
    COM_CXXFLAGS += -wd1682
endif
endif

AM_LFLAGS = -olex.yy.c
AM_YFLAGS = -d

BUILT_SOURCES = AsciiParser.hpp MathParser.hpp

lib_LTLIBRARIES    = libLatAnalyze.la
noinst_LTLIBRARIES = libLexers.la

libLexers_la_SOURCES  = AsciiLexer.lpp MathLexer.lpp
if CXX_GNU
    libLexers_la_CXXFLAGS = $(COM_CXXFLAGS) -Wno-unused-function
else
    libLexers_la_CXXFLAGS = $(COM_CXXFLAGS)
endif

libLatAnalyze_la_SOURCES = \
    AsciiFile.cpp          \
    AsciiParser.ypp        \
    CompiledFunction.cpp   \
    CompiledModel.cpp      \
    Derivative.cpp         \
    Exceptions.cpp         \
    File.cpp               \
    FitInterface.cpp       \
    Function.cpp           \
    Global.cpp             \
    GslHybridRootFinder.cpp\
    GslQagsIntegrator.cpp  \
    Hdf5File.cpp           \
    Histogram.cpp          \
    includes.hpp           \
    Io.cpp                 \
    Mat.cpp                \
    Math.cpp               \
    MathInterpreter.cpp    \
    MathParser.ypp         \
    Minimizer.cpp          \
    Model.cpp              \
    Plot.cpp               \
    RootFinder.cpp         \
    Solver.cpp             \
    StatArray.cpp          \
    TabFunction.cpp        \
    XYSampleData.cpp       \
    XYStatData.cpp         \
	../config.h
libLatAnalyze_ladir = $(pkgincludedir)
libLatAnalyze_la_HEADERS = \
    AsciiFile.hpp          \
    CompiledFunction.hpp   \
    CompiledModel.hpp      \
    Dataset.hpp            \
    Derivative.hpp         \
    Exceptions.hpp         \
    Function.hpp           \
    File.hpp               \
    FitInterface.hpp       \
    Global.hpp             \
    GslHybridRootFinder.hpp\
    GslQagsIntegrator.hpp  \
    Hdf5File.hpp           \
    Histogram.hpp          \
    Integrator.hpp         \
    Io.hpp                 \
    IoObject.hpp           \
    Mat.hpp                \
    Math.hpp               \
    MathInterpreter.hpp    \
    MatSample.hpp          \
    Minimizer.hpp          \
    Model.hpp              \
    ParserState.hpp        \
    Plot.hpp               \
    RootFinder.hpp         \
    TabFunction.hpp        \
    Solver.hpp             \
    StatArray.hpp          \
    XYSampleData.hpp       \
    XYStatData.hpp
if HAVE_MINUIT
    libLatAnalyze_la_SOURCES += MinuitMinimizer.cpp
    libLatAnalyze_la_HEADERS += MinuitMinimizer.hpp
endif
if HAVE_NLOPT
    libLatAnalyze_la_SOURCES += NloptMinimizer.cpp
    libLatAnalyze_la_HEADERS += NloptMinimizer.hpp
endif
libLatAnalyze_la_CXXFLAGS = $(COM_CXXFLAGS)
libLatAnalyze_la_LIBADD   = libLexers.la

ACLOCAL_AMFLAGS = -I .buildutils/m4
