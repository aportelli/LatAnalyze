#!/usr/bin/env bash

NAME='nlopt-2.4.2'

if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix> {osx|linux}" 1>&2
  exit 1
fi
PREFIX=$1

set -ex
INITDIR=`pwd`
cd local/build
wget http://ab-initio.mit.edu/nlopt/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir -p ${NAME}/build
cd ${NAME}/build
../configure --prefix=${PREFIX} --with-cxx --without-guile --without-python --without-octave --without-matlab --with-pic
make -j4 
make install
cd ${INITDIR}/local
touch .built.nlopt
cd ${INITDIR}
