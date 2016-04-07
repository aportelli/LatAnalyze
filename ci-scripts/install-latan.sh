#!/usr/bin/env bash

if (( $# != 2 )); then
  echo "usage: `basename $0` <prefix> {osx|linux}" 1>&2
  exit 1
fi
LATAN_PREFIX=$1
OS=$2

set -ex
PREFIX=`pwd`/local
./install-deps.sh
cd ..
./bootstrap.sh
mkdir -p build
cd build
if [[ "$OS" == "osx" ]]; then EXTRA_FLAGS='--with-gsl=/usr/local'; fi
../configure --prefix=$LATAN_PREFIX --with-minuit=$PREFIX --with-nlopt=$PREFIX --with-latcore=$PREFIX --with-hdf5=$PREFIX $EXTRA_FLAGS
make -j4
make install
