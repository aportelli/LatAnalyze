if CXX_GNU
    COM_CXXFLAGS = -Wall -W -pedantic -Wno-deprecated-declarations
else
if CXX_INTEL
    COM_CXXFLAGS = -wd1682 -Wall
endif
endif

bin_PROGRAMS =            \
    latan-plot            \
    latan-sample-combine  \
    latan-sample-dwt      \
    latan-sample-element  \
    latan-sample-fake     \
    latan-sample-ft       \
    latan-sample-plot     \
    latan-sample-plot-corr\
    latan-sample-read     \
    latan-resample        

latan_plot_SOURCES  = plot.cpp
latan_plot_CXXFLAGS = $(COM_CXXFLAGS)
latan_plot_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_combine_SOURCES  = sample-combine.cpp
latan_sample_combine_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_combine_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_dwt_SOURCES  = sample-dwt.cpp
latan_sample_dwt_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_dwt_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_element_SOURCES  = sample-element.cpp
latan_sample_element_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_element_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_fake_SOURCES  = sample-fake.cpp
latan_sample_fake_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_fake_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_ft_SOURCES  = sample-ft.cpp
latan_sample_ft_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_ft_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_plot_corr_SOURCES  = sample-plot-corr.cpp
latan_sample_plot_corr_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_plot_corr_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_plot_SOURCES  = sample-plot.cpp
latan_sample_plot_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_plot_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_sample_read_SOURCES  = sample-read.cpp
latan_sample_read_CXXFLAGS = $(COM_CXXFLAGS)
latan_sample_read_LDFLAGS  = -L../lib/.libs -lLatAnalyze

latan_resample_SOURCES  = resample.cpp
latan_resample_CXXFLAGS = $(COM_CXXFLAGS)
latan_resample_LDFLAGS  = -L../lib/.libs -lLatAnalyze

ACLOCAL_AMFLAGS = -I .buildutils/m4
