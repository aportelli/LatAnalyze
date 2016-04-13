#!/usr/bin/env bash

if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix> {osx|linux}" 1>&2
  exit 1
fi
PREFIX=$1

set -ex
INITDIR=`pwd`
cd local/build
if [ -d LatCore ]; then
  cd LatCore
  git pull origin master
else  
  git clone https://github.com/aportelli/LatCore.git
  mkdir LatCore/build
  cd LatCore
  ./bootstrap.sh
fi
cd build
../configure --prefix=${PREFIX}
make -j4 
make install
cd ${INITDIR}
