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

noinst_PROGRAMS = \
    ex_test

ex_test_SOURCES = ex_test.cpp
ex_test_CFLAGS  = -g -O2
ex_test_LDFLAGS = -L../latan/.libs -llatan

ACLOCAL_AMFLAGS = -I .buildutils/m4
