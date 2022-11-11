if CXX_GNU
    COM_CXXFLAGS = -Wall -W -pedantic -Wno-deprecated-declarations
else
if CXX_INTEL
    COM_CXXFLAGS = -wd1682 -Wall
endif
endif

bin_PROGRAMS = latan-2pt-fit latan-2pt-effmass

latan_2pt_fit_SOURCES  = 2pt-fit.cpp
latan_2pt_fit_CXXFLAGS = $(COM_CXXFLAGS)
latan_2pt_fit_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_2pt_effmass_SOURCES  = 2pt-effmass.cpp
latan_2pt_effmass_CXXFLAGS = $(COM_CXXFLAGS)
latan_2pt_effmass_LDFLAGS  = -L../lib/.libs -lLatAnalyze

ACLOCAL_AMFLAGS = -I .buildutils/m4
