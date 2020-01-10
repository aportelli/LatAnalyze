#!/usr/bin/env bash

if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix>" 1>&2
  exit 1
fi
PREFIX=$1

set -ex
INITDIR=$(pwd -P)
cd ${PREFIX}
PREFIX=$(pwd -P)
cd ${INITDIR}
./install-deps.sh ${PREFIX}
cd ..
./bootstrap.sh
mkdir -p build
cd build
../configure --prefix=${PREFIX} --with-minuit=${PREFIX} --with-nlopt=${PREFIX} --with-hdf5=${PREFIX} --with-gsl=${PREFIX} CXXFLAGS="${CXXFLAGS} -O3 -march=haswell -mtune=haswell"
make -j4
make install
