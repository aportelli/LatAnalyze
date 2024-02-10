#!/usr/bin/env bash

name='2.6.1'

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
wget "https://github.com/stevengj/nlopt/archive/v${name}.tar.gz"
tar -xzvf v${name}.tar.gz
name=nlopt-${name}
mkdir -p ${name}/build
cd ${name}/build
cmake -DCMAKE_INSTALL_PREFIX="${prefix}" -DCMAKE_BUILD_WITH_INSTALL_NAME_DIR=TRUE \
  -DCMAKE_INSTALL_NAME_DIR="${prefix}/lib" -DNLOPT_PYTHON=OFF -DNLOPT_OCTAVE=OFF \
  -DNLOPT_MATLAB=OFF -DNLOPT_GUILE=OFF -DNLOPT_SWIG=OFF ..
make -j "${ntasks}"
make install
cd "${init_dir}/local"
touch .built.nlopt
cd "${init_dir}"
