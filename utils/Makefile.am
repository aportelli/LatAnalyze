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

noinst_PROGRAMS =           \
    latan_sample_read

latan_sample_read_SOURCES = sample_read.cpp
latan_sample_read_CFLAGS  = -g -O2
latan_sample_read_LDFLAGS = -L../latan/.libs -llatan

ACLOCAL_AMFLAGS = -I .buildutils/m4
