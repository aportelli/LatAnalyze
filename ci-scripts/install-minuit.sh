#!/usr/bin/env bash

NAME='Minuit2-5.34.14'

set -ex
INITDIR=`pwd`
cd local/build
wget http://www.cern.ch/mathlibs/sw/5_34_14/Minuit2/${NAME}.tar.gz
tar -xzvf ${NAME}.tar.gz
mkdir -p ${NAME}/build
cd ${NAME}/build
../configure --prefix=${INITDIR}/local --disable-openmp
make -j4 
make install
cd ${INITDIR}/local
touch .built.minuit
cd ${INITDIR}
