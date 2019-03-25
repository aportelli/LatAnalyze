COM_CXXFLAGS = -Wall
if CXX_GNU
    COM_CXXFLAGS += -W -pedantic -Wno-deprecated-declarations
else
if CXX_INTEL
    COM_CXXFLAGS += -wd1682
endif
endif

include eigen_files.mk

AM_LFLAGS = -olex.yy.c
AM_YFLAGS = -d

lib_LTLIBRARIES    = libLatAnalyze.la
noinst_LTLIBRARIES = libLexers.la

libLexers_la_SOURCES  = Io/AsciiLexer.lpp Core/MathLexer.lpp
if CXX_GNU
    libLexers_la_CXXFLAGS = $(COM_CXXFLAGS) -Wno-unused-parameter -Wno-unused-function -Wno-deprecated-register
else
    libLexers_la_CXXFLAGS = $(COM_CXXFLAGS)
endif

libLatAnalyze_la_SOURCES =           \
    includes.hpp                     \
    Global.cpp                       \
    Core/Exceptions.cpp              \
    Core/Mat.cpp                     \
    Core/Math.cpp                    \
    Core/MathInterpreter.cpp         \
    Core/MathParser.ypp              \
    Core/OptParser.cpp               \
    Core/Plot.cpp                    \
    Core/Utilities.cpp               \
    Functional/CompiledFunction.cpp  \
    Functional/CompiledModel.cpp     \
    Functional/Function.cpp          \
    Functional/Model.cpp             \
    Functional/TabFunction.cpp       \
    Io/AsciiFile.cpp                 \
    Io/AsciiParser.ypp               \
    Io/BinReader.cpp                 \
    Io/File.cpp                      \
    Io/Hdf5File.cpp                  \
    Io/Io.cpp                        \
    Io/XmlReader.cpp                 \
    Io/Xml/tinyxml2.cpp              \
    Numerical/Derivative.cpp         \
    Numerical/GslFFT.cpp             \
    Numerical/GslHybridRootFinder.cpp\
    Numerical/GslMinimizer.cpp       \
    Numerical/GslQagsIntegrator.cpp  \
    Numerical/Minimizer.cpp          \
    Numerical/RootFinder.cpp         \
    Numerical/Solver.cpp             \
    Statistics/FitInterface.cpp      \
    Statistics/Histogram.cpp         \
    Statistics/Random.cpp            \
    Statistics/StatArray.cpp         \
    Statistics/XYSampleData.cpp      \
    Statistics/XYStatData.cpp        \
	../config.h
libLatAnalyze_ladir = $(pkgincludedir)
HPPFILES =                           \
    Global.hpp                       \
    Core/Eigen.hpp                   \
    Core/EigenPlugin.hpp             \
    Core/Exceptions.hpp              \
    Core/Mat.hpp                     \
    Core/Math.hpp                    \
    Core/MathInterpreter.hpp         \
    Core/OptParser.hpp               \
    Core/ParserState.hpp             \
    Core/Plot.hpp                    \
    Core/stdincludes.hpp             \
    Core/Utilities.hpp               \
    Functional/CompiledFunction.hpp  \
    Functional/CompiledModel.hpp     \
    Functional/Function.hpp          \
    Functional/Model.hpp             \
    Functional/TabFunction.hpp       \
    Io/AsciiFile.hpp                 \
    Io/BinReader.hpp                 \
    Io/File.hpp                      \
    Io/Hdf5File.hpp                  \
    Io/Io.hpp                        \
    Io/IoObject.hpp                  \
    Io/XmlReader.hpp                 \
    Numerical/Derivative.hpp         \
    Numerical/FFT.hpp                \
    Numerical/GslFFT.hpp             \
    Numerical/GslHybridRootFinder.hpp\
    Numerical/GslMinimizer.hpp       \
    Numerical/GslQagsIntegrator.hpp  \
    Numerical/Integrator.hpp         \
    Numerical/Minimizer.hpp          \
    Numerical/RootFinder.hpp         \
    Numerical/Solver.hpp             \
    Statistics/Dataset.hpp           \
    Statistics/FitInterface.hpp      \
    Statistics/Histogram.hpp         \
    Statistics/MatSample.hpp         \
    Statistics/Random.hpp            \
    Statistics/StatArray.hpp         \
    Statistics/XYSampleData.hpp      \
    Statistics/XYStatData.hpp
if HAVE_MINUIT
    libLatAnalyze_la_SOURCES += Numerical/MinuitMinimizer.cpp
    HPPFILES += Numerical/MinuitMinimizer.hpp
endif
if HAVE_NLOPT
    libLatAnalyze_la_SOURCES += Numerical/NloptMinimizer.cpp
    HPPFILES += Numerical/NloptMinimizer.hpp
endif
libLatAnalyze_la_CXXFLAGS = $(COM_CXXFLAGS)
libLatAnalyze_la_LIBADD   = libLexers.la

if HAVE_AM_MINOR_LE_11
Io/AsciiParser.hpp: Io/AsciiParser.ypp
	$(AM_V_YACC) $(YACC) -o Io/AsciiParser.cpp --defines=Io/AsciiParser.hpp $<

Core/MathParser.hpp: Core/MathParser.ypp
	$(AM_V_YACC) $(YACC) -o Core/MathParser.cpp --defines=Core/MathParser.hpp $<
endif

BUILT_SOURCES = Io/AsciiParser.hpp Core/MathParser.hpp

CLEANFILES =           \
    Core/MathLexer.cpp \
    Core/MathParser.cpp\
    Core/MathParser.hpp\
    Io/AsciiLexer.cpp  \
    Io/AsciiParser.cpp \
    Io/AsciiParser.hpp

nobase_dist_pkginclude_HEADERS = $(HPPFILES) $(eigen_files) Io/Xml/tinyxml2.hpp

ACLOCAL_AMFLAGS = -I .buildutils/m4
