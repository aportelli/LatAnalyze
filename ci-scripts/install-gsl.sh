#!/usr/bin/env bash

NAME='gsl-2.6'

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
cd ${INITDIR}/local/build
wget http://ftpmirror.gnu.org/gsl/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir -p ${NAME}/build
cd ${NAME}/build
../configure --prefix=${PREFIX}
make -j${NTASKS}
make install
cd ${INITDIR}/local
touch .built.gsl
cd ${INITDIR}