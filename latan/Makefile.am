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
nobase_dist_pkginclude_HEADERS = $(eigen_files)

BUILT_SOURCES = AsciiParser.hpp MathParser.hpp

lib_LTLIBRARIES = liblatan.la
	
liblatan_la_SOURCES =   \
    AsciiFile.cpp       \
    AsciiParser.ypp     \
    AsciiLexer.lpp      \
    CompiledFunction.cpp\
    Exceptions.cpp      \
    Function.cpp        \
    Global.cpp          \
    includes.hpp        \
    File.cpp            \
    Mat.cpp             \
    Math.cpp            \
    MathInterpreter.cpp \
    MathParser.ypp      \
    MathLexer.lpp       \
    Minimizer.cpp       \
    Plot.cpp            \
    RandGen.cpp         \
    Sample.cpp          \
	../config.h
liblatan_ladir = $(pkgincludedir)
liblatan_la_HEADERS =   \
    AsciiFile.hpp       \
    CompiledFunction.hpp\
    Dataset.hpp         \
    Function.hpp        \
    Global.hpp          \
    File.hpp            \
    IoObject.hpp        \
    Mat.hpp             \
    Math.hpp            \
    MathInterpreter.hpp \
    Minimizer.hpp       \
    ParserState.hpp     \
    Plot.hpp            \
    RandGen.hpp         \
    Sample.hpp          \
    StatArray.hpp
    
if HAVE_MINUIT
liblatan_la_SOURCES += MinuitMinimizer.cpp
liblatan_la_HEADERS += MinuitMinimizer.hpp
endif

liblatan_la_CFLAGS = $(COM_CFLAGS)
liblatan_la_CXXFLAGS = $(COM_CXXFLAGS)

ACLOCAL_AMFLAGS = -I .buildutils/m4
