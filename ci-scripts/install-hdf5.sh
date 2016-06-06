#!/usr/bin/env bash

NAME='hdf5-1.8.16'

if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix> {osx|linux}" 1>&2
  exit 1
fi
PREFIX=$1

set -ex
INITDIR=`pwd`
cd local/build
wget http://www.hdfgroup.org/ftp/HDF5/releases/${NAME}/src/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir ${NAME}/build
cd ${NAME}/build
../configure --prefix=${PREFIX} --enable-cxx
make -j4 
make install
cd ${INITDIR}/local
touch .built.hdf5
cd ${INITDIR}
