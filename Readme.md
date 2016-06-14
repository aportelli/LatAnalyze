# LatAnalyze 
Contributors: Antonin Portelli, Matt Spraggs

License: GNU General Public License v3

<table>
<tr>
    <td>Last stable release</td>
    <td><a href="https://travis-ci.org/aportelli/LatAnalyze">
    <img src="https://travis-ci.org/aportelli/LatAnalyze.svg?branch=master"></a>
    </td>
</tr>
<tr>
    <td>Development branch</td>
    <td><a href="https://travis-ci.org/aportelli/LatAnalyze">
    <img src="https://travis-ci.org/aportelli/LatAnalyze.svg?branch=develop"></a>
    </td>
</tr>
</table>

## Description
LatAnalyze is a C++11 library for statistical data analysis based on bootstrap
resampling. It has been written with lattice QCD data analysis in mind (hence
the name), but its features are not lattice specific and can be used more general statistical context.

Sadly a proper documentation was never written, but some comprehensive examples covering most features can be found in the `examples` directory.

The main features are the following:

* Array and matrix types with fast arithmetic operations based on [Eigen](http://eigen.tuxfamily.org).
* High-level types for bootstrap sample manipulation (including various statistical estimators and histogramming).
* Mathematical expression parser for runtime defined functions.
* Data I/O in ASCII and HDF5 (optional).
* High-level wrappers to minimisation routines from the [GSL](http://www.gnu.org/software/gsl/), [Minuit](http://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/) (optional) and [NLopt](http://ab-initio.mit.edu/wiki/index.php/NLopt).
* Non-linear regression with error on independent variables (through total least squares).
* High-level wrappers to numerical integrator and non-linear solver from the [GSL](http://www.gnu.org/software/gsl/).
* High-level functional types for function of model. General functions can be defined from C pointers, C++ objects, strings of mathematical expressions or tabulated data.
* High-level plotting functions based on [gnuplot](http://www.gnuplot.info), with the possibility of generating and saving plot scripts.

## Installation
The head of the `master` branch always points to the latest stable release. The `develop` branch is the main unstable branch of LatAnalyze.

LatAnalyze is written in C++11 and requires a rather recent C++ compiler to be built. It has been successfully built on various Linux and OS X platforms using clang (from 3.7), GCC (from 4.9) and the Intel C++ compiler (2016).
The strict dependencies are the [GSL](http://www.gnu.org/software/gsl/) and [LatCore](https://github.com/aportelli/LatCore).
Additionally, autoconf, automake (from 1.11), libtool, bison (from 3.0) and flex are necessary to build the library. Unless you use a very exotic system, these tools are standard on any Unix platform and should be already present or easy to install through a package manager.
Optional dependencies are [HDF5](https://www.hdfgroup.org/HDF5/) (built with C++ support), [Minuit](http://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/) and [NLopt](http://ab-initio.mit.edu/wiki/index.php/NLopt).


For a quick installation with all possible extensions execute: 
```
./install-latan.sh <prefix> {osx|linux}
``` 
in the `ci-scripts` directory where `<prefix>` is where you want LatAnalyze (and its dependencies) to be installed and `{osx|linux}` should be the name of your OS. This script will automatically download, build and install LatCore, HDF5, Minuit and NLopt. It is assumed that the GSL can be found in a standard location (_e.g._ it has been installed through a package manager for Linux and is present in `/usr/local` for OS X).

For a more customised installation, one first needs to generate the build system by running `./bootstrap.sh` in the root directory. Then the library can be built and installed through the usual GNU mantra `./configure <options> && make && make install`. Use `./configure --help` to obtain a list of possible options for `./configure`. Because Eigen expressions rely a lot on inlining and compiler optimisations it is strongly recommended to set the `CXXFLAGS` variable to `-O3 -march=native -mtune=native`.

## History
#### v3.2 (needs LatCore 1.1)
Additions:
* 2-pt function fitter `latan-2pt-fit`
* Tool to extract one element of a matrix sample `latan-sample-element`
* Band plotting

Changes:
* Sample utilities renamed `latan-sample-*`
* Resample utility renamed `latan-resample`

#### v3.1.2
Fixes:
* HDF5 archive URL update in build scripts

#### v3.1.1 (needs LatCore 1.0)
Fixes:
* Minuit precision fixed
* Minor fit interface fixes

#### v3.1
Additions:
* Wrappers to NLopt and GSL minimisers.
* Command-line tool to plot the correlation heatmap of a boostrap sample file.
* I/O functions for `DSample` type.

Changes:
* Internal random generator removed (obsolete because of C++11 pseudo-random generators).
* Fit interface and `XY*Data` classes rewritten from scratch for improved flexibility and performance.

Fixes:
* **Loads** of portability and compatibility fixes and [CI with Travis](https://travis-ci.org/aportelli/LatAnalyze).

#### v3.0
Commit `7b4f2884a5e99bbfab4d4bd7623f609a55403c39`.  
First 'stable' version of LatAnalyze in C++. The v2.0 refers to the [C version](https://github.com/aportelli/LatAnalyze-legacy) and v1.0 to an old undistributed version.  
**This version compiles fine on OS X with clang but does have many portability issues to other platforms/compilers, v3.1 is the first real release.**