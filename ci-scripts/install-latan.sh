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
cd "${init_dir}"
./install-deps.sh "${prefix}" "${ntasks}"
mkdir -p "${init_dir}/local/build/latan"
cd "${init_dir}/local/build/latan"
cmake -DCMAKE_INSTALL_PREFIX="${prefix}" -DCMAKE_PREFIX_PATH="${prefix}" \
  -DCMAKE_BUILD_TYPE="RelWithDebInfo" "${init_dir}/.."
make -j "${ntasks}"
make install
