#!/usr/bin/env bash

if (($# != 2)); then
  echo "usage: $(basename "$0") <prefix> <ntasks>" 1>&2
  exit 1
fi
prefix=$1
ntasks=$2

set -ex
mkdir -p local/build
for d in gsl nlopt minuit hdf5; do
  if [ ! -e local/.built.${d} ]; then
    ./install-${d}.sh "${prefix}" "${ntasks}"
  fi
done
