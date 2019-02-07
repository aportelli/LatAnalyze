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

lib_LTLIBRARIES    = libLatAnalyze.la
noinst_LTLIBRARIES = libLexers.la

libLexers_la_SOURCES  = AsciiLexer.lpp MathLexer.lpp
if CXX_GNU
    libLexers_la_CXXFLAGS = $(COM_CXXFLAGS) -Wno-unused-parameter -Wno-unused-function -Wno-deprecated-register
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
    GslFFT.cpp             \
    GslHybridRootFinder.cpp\
    GslMinimizer.cpp       \
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
    FFT.hpp                \
    Function.hpp           \
    File.hpp               \
    FitInterface.hpp       \
    Global.hpp             \
    GslFFT.hpp             \
    GslHybridRootFinder.hpp\
    GslMinimizer.hpp       \
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

if HAVE_AM_MINOR_LE_11
AsciiParser.hpp: AsciiParser.ypp
	$(AM_V_YACC) $(YACC) -o AsciiParser.cpp --defines=AsciiParser.hpp $<

MathParser.hpp: MathParser.ypp
	$(AM_V_YACC) $(YACC) -o MathParser.cpp --defines=MathParser.hpp $<
endif

BUILT_SOURCES = AsciiParser.hpp MathParser.hpp

CLEANFILES =       \
    MathLexer.cpp  \
    AsciiLexer.cpp \
    AsciiParser.cpp\
    AsciiParser.hpp\
    MathParser.cpp \
    MathParser.hpp

ACLOCAL_AMFLAGS = -I .buildutils/m4
