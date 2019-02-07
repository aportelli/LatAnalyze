#!/usr/bin/env bash

if (( $# != 2 )); then
  echo "usage: `basename $0` <prefix> {osx|linux}" 1>&2
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
if [[ "$OS" == "osx" ]]; then EXTRA_FLAGS='--with-gsl=/usr/local'; fi
../configure --prefix=$PREFIX --with-minuit=$PREFIX --with-nlopt=$PREFIX --with-latcore=$PREFIX --with-hdf5=$PREFIX $EXTRA_FLAGS CXXFLAGS="${CXXFLAGS} -O3 -march=native -mtune=native"
make -j4
make install
