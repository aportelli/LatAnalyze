#!/usr/bin/env bash

if (( $# != 2 )); then
  echo "usage: `basename $0` <prefix> <ntasks>" 1>&2
  exit 1
fi
PREFIX=$1
NTASKS=$2

set -ex
INITDIR=$(pwd -P)
mkdir -p ${PREFIX}
cd ${PREFIX}
PREFIX=$(pwd -P)
cd ${INITDIR}
./install-deps.sh ${PREFIX} ${NTASKS}
cd ..
./bootstrap.sh
mkdir -p build
cd build
../configure --prefix=${PREFIX} --with-minuit=${PREFIX} --with-nlopt=${PREFIX} --with-hdf5=${PREFIX} --with-gsl=${PREFIX} CXXFLAGS="${CXXFLAGS} -O3 -march=native -mtune=native"
make -j${NTASKS}
make install
