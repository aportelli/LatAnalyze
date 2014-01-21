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

BUILT_SOURCES = IOASCIIParser.hpp MathParser.hpp

lib_LTLIBRARIES = liblatan.la
	
liblatan_la_SOURCES =  \
    Exceptions.cpp     \
    Global.cpp         \
    includes.hpp       \
    IO.cpp             \
    IOASCIIParser.ypp  \
    IOASCIILexer.lpp   \
    Mat.cpp            \
    MathCompiler.cpp   \
    MathParser.ypp     \
    MathLexer.lpp      \
    Sample.cpp         \
	../config.h
liblatan_ladir = $(pkgincludedir)
liblatan_la_HEADERS = \
    Global.hpp        \
    IO.hpp            \
    IOObject.hpp      \
    Mat.hpp           \
    MathCompiler.hpp  \
    Sample.hpp
liblatan_la_CFLAGS = $(COM_CFLAGS)
liblatan_la_CXXFLAGS = $(COM_CXXFLAGS)

ACLOCAL_AMFLAGS = -I .buildutils/m4
