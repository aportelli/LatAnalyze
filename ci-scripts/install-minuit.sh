#!/usr/bin/env bash

NAME='Minuit2-5.34.14'

if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix>" 1>&2
  exit 1
fi
PREFIX=$1

set -ex
INITDIR=$(pwd -P)
cd ${PREFIX}
PREFIX=$(pwd -P)
cd ${INITDIR}/local/build
wget http://www.cern.ch/mathlibs/sw/5_34_14/Minuit2/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir -p ${NAME}/build
cd ${NAME}/build
../configure --prefix=${PREFIX} --disable-openmp
make -j4 
make install
cd ${INITDIR}/local
touch .built.minuit
cd ${INITDIR}
