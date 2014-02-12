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

BUILT_SOURCES = IoAsciiParser.hpp MathParser.hpp

lib_LTLIBRARIES = liblatan.la
	
liblatan_la_SOURCES =   \
    CompiledFunction.cpp\
    Exceptions.cpp      \
    Function.cpp        \
    Global.cpp          \
    includes.hpp        \
    Io.cpp              \
    IoAsciiParser.ypp   \
    IoAsciiLexer.lpp    \
    Mat.cpp             \
    Math.cpp            \
    MathInterpreter.cpp \
    MathParser.ypp      \
    MathLexer.lpp       \
    Plot.cpp            \
    RandGen.cpp         \
    Sample.cpp          \
	../config.h
liblatan_ladir = $(pkgincludedir)
liblatan_la_HEADERS =   \
    CompiledFunction.hpp\
    Dataset.hpp         \
    Function.hpp        \
    Global.hpp          \
    Io.hpp              \
    IoObject.hpp        \
    Mat.hpp             \
    Math.hpp            \
    MathInterpreter.hpp \
    ParserState.hpp     \
    Plot.hpp            \
    RandGen.hpp         \
    Sample.hpp          \
    StatArray.hpp
liblatan_la_CFLAGS = $(COM_CFLAGS)
liblatan_la_CXXFLAGS = $(COM_CXXFLAGS)

ACLOCAL_AMFLAGS = -I .buildutils/m4
