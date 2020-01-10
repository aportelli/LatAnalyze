#!/usr/bin/env bash

NAME='gsl-2.6'

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
cd local/build
wget http://ftpmirror.gnu.org/gsl/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir -p ${NAME}/build
cd ${NAME}/build
../configure --prefix=${PREFIX}
make -j4 
make install
cd ${INITDIR}/local
touch .built.gsl
cd ${INITDIR}