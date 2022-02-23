#!/usr/bin/env bash

NAME='2.6.1'

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
wget https://github.com/stevengj/nlopt/archive/v${NAME}.tar.gz
tar -xzvf v${NAME}.tar.gz
NAME=nlopt-${NAME}
mkdir -p ${NAME}/build
cd ${NAME}/build
cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_BUILD_WITH_INSTALL_NAME_DIR=TRUE -DCMAKE_INSTALL_NAME_DIR="${PREFIX}/lib" ..
make -j${NTASKS}
make install
cd ${INITDIR}/local
touch .built.nlopt
cd ${INITDIR}
