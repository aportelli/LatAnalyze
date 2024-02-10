#!/usr/bin/env bash

name='hdf5-1.14.3'

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
wget "https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.14/${name}/src/${name}.tar.gz"
tar -xzvf ${name}.tar.gz
mkdir ${name}/build
cd ${name}/build
../configure --prefix="${prefix}" --enable-cxx
make -j "${ntasks}"
make install
cd "${init_dir}/local"
touch .built.hdf5
cd "${init_dir}"
