#!/usr/bin/env bash

NAME='hdf5-1.8.16'

set -ex
INITDIR=`pwd`
cd local/build
wget http://www.hdfgroup.org/ftp/HDF5/current/src/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir ${NAME}/build
cd ${NAME}/build
../configure --prefix=${INITDIR}/local --enable-cxx
make -j4 
make install
cd ${INITDIR}/local
touch .built.hdf5
cd ${INITDIR}
