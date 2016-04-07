#!/usr/bin/env bash

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
../configure --prefix=${INITDIR}/local
make -j4 
make install
cd ${INITDIR}
