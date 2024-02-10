#!/usr/bin/env bash

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
rm -rf root
git clone https://github.com/root-project/root.git
cd root/math/minuit2/
mkdir build
cd build
cmake .. -Dminuit2_standalone=ON -DCMAKE_INSTALL_PREFIX="${prefix}"
make -j "${ntasks}"
make install
cd "${init_dir}/local"
touch .built.minuit
cd "${init_dir}"
