if CXX_GNU
    COM_CXXFLAGS = -Wall -W -pedantic -Wno-deprecated-declarations
else
if CXX_INTEL
    COM_CXXFLAGS = -wd1682 -Wall
endif
endif

bin_PROGRAMS =            \
    latan_make_fake_sample\
    latan_sample_combine  \
    latan_sample_element  \
    latan_sample_plot_corr\
    latan_sample_read     \
    latan_resample

latan_make_fake_sample_SOURCES  = make_fake_sample.cpp
latan_make_fake_sample_CXXFLAGS = $(COM_CXXFLAGS)
latan_make_fake_sample_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_combine_SOURCES  = sample_combine.cpp
latan_sample_combine_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_combine_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_element_SOURCES  = sample_element.cpp
latan_sample_element_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_element_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_plot_corr_SOURCES  = sample_plot_corr.cpp
latan_sample_plot_corr_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_plot_corr_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_read_SOURCES  = sample_read.cpp
latan_sample_read_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_read_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_resample_SOURCES  = resample.cpp
latan_resample_CXXFLAGS = $(COM_CXXFLAGS)
latan_resample_LDFLAGS  = -L../lib/.libs -lLatAnalyze

ACLOCAL_AMFLAGS = -I .buildutils/m4
