#!/usr/bin/env bash

name='gsl-2.6'

if (($# != 2)); then
  echo "usage: $(basename "$0") <prefix> <ntasks>" 1>&2
  exit 1
fi
prefix=$1
ntasks=$2

set -ex
init_dir=$(pwd -P)
mkdir -p "${prefix}"
cd "${prefix}"
prefix=$(pwd -P)
cd "${init_dir}/local/build"
wget http://ftpmirror.gnu.org/gsl/${name}.tar.gz
tar -xzvf ${name}.tar.gz
mkdir -p ${name}/build
cd ${name}/build
../configure --prefix="${prefix}"
make -j"${ntasks}"
make install
cd "${init_dir}/local"
touch .built.gsl
cd "${init_dir}"
