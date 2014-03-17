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

AM_LFLAGS = -olex.yy.c
AM_YFLAGS = -d

include eigen_files.mk
nobase_dist_pkginclude_HEADERS = $(eigen_files) XML/tinyxml2.hpp

BUILT_SOURCES = AsciiParser.hpp MathParser.hpp

lib_LTLIBRARIES = libLatAnalyze.la
	
libLatAnalyze_la_SOURCES =\
    AsciiFile.cpp         \
    AsciiParser.ypp       \
    AsciiLexer.lpp        \
    Chi2Function.cpp      \
    CompiledFunction.cpp  \
    CompiledModel.cpp     \
    Exceptions.cpp        \
    Function.cpp          \
    Global.cpp            \
    includes.hpp          \
    File.cpp              \
    FitInterface.cpp      \
    Mat.cpp               \
    Math.cpp              \
    MathInterpreter.cpp   \
    MathParser.ypp        \
    MathLexer.lpp         \
    MatSample.cpp         \
    Minimizer.cpp         \
    Model.cpp             \
    Plot.cpp              \
    RandGen.cpp           \
    XmlReader.cpp         \
    XYSampleData.cpp      \
    XYStatData.cpp        \
    XML/tinyxml2.cpp      \
	../config.h
libLatAnalyze_ladir = $(pkgincludedir)
libLatAnalyze_la_HEADERS =\
    AsciiFile.hpp         \
    Chi2Function.hpp      \
    CompiledFunction.hpp  \
    CompiledModel.hpp     \
    Dataset.hpp           \
    Exceptions.hpp        \
    FitInterface.hpp      \
    Function.hpp          \
    Global.hpp            \
    File.hpp              \
    IoObject.hpp          \
    Mat.hpp               \
    Math.hpp              \
    MathInterpreter.hpp   \
    MatSample.hpp         \
    Minimizer.hpp         \
    Model.hpp             \
    ParserState.hpp       \
    Plot.hpp              \
    RandGen.hpp           \
    StatArray.hpp         \
    XmlReader.hpp         \
    XYSampleData.hpp      \
    XYStatData.hpp
if HAVE_MINUIT
libLatAnalyze_la_SOURCES += MinuitMinimizer.cpp
libLatAnalyze_la_HEADERS += MinuitMinimizer.hpp
endif

libLatAnalyze_la_CFLAGS   = $(COM_CFLAGS)
libLatAnalyze_la_CXXFLAGS = $(COM_CXXFLAGS)

ACLOCAL_AMFLAGS = -I .buildutils/m4
