#!/usr/bin/env bash

NAME='hdf5-1.10.8'

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
wget https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.10/${NAME}/src/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir ${NAME}/build
cd ${NAME}/build
../configure --prefix=${PREFIX} --enable-cxx
make -j${NTASKS}
make install
cd ${INITDIR}/local
touch .built.hdf5
cd ${INITDIR}
