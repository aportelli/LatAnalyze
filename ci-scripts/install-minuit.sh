#!/usr/bin/env bash


if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix>" 1>&2
  exit 1
fi
PREFIX=$1

set -ex
INITDIR=$(pwd -P)
mkdir -p ${PREFIX}
cd ${PREFIX}
PREFIX=$(pwd -P)
cd ${INITDIR}/local/build
rm -rf root
git clone https://github.com/root-project/root.git
cd root/math/minuit2/
mkdir build; cd build
cmake .. -Dminuit2_standalone=ON -DCMAKE_INSTALL_PREFIX=${PREFIX}
make -j4 
make install
cd ${INITDIR}/local
touch .built.minuit
cd ${INITDIR}
