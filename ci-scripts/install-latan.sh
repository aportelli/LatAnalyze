#!/usr/bin/env bash

if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix>" 1>&2
  exit 1
fi
PREFIX=$1
OS=$2

set -ex
./install-deps.sh ${PREFIX}
cd ..
./bootstrap.sh
mkdir -p build
cd build
../configure --prefix=$PREFIX --with-minuit=$PREFIX --with-nlopt=$PREFIX --with-latcore=$PREFIX --with-hdf5=$PREFIX --with-gsl=$PREFIX CXXFLAGS="${CXXFLAGS} -O3 -march=native -mtune=native"
make -j4
make install
