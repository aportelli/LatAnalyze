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

BUILT_SOURCES = IOASCIIParser.hpp

lib_LTLIBRARIES = liblatan.la
	
liblatan_la_SOURCES =  \
    Exceptions.cpp     \
    Global.cpp         \
    includes.hpp       \
    IO.cpp             \
    IOASCIIParser.ypp  \
    IOASCIILexer.lpp   \
    IOObject.cpp       \
    Mat.cpp            \
    Sample.cpp         \
	../config.h
liblatan_ladir = $(includedir)/latan
liblatan_la_HEADERS = \
    Global.hpp        \
    IO.hpp            \
    IOObject.hpp      \
    Mat.hpp           \
    Sample.hpp
liblatan_la_CFLAGS = $(COM_CFLAGS)
liblatan_la_CXXFLAGS = $(COM_CXXFLAGS)

ACLOCAL_AMFLAGS = -I .buildutils/m4
