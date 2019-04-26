#!/usr/bin/env bash

if (( $# != 1 )); then
  echo "usage: `basename $0` <prefix>" 1>&2
  exit 1
fi
PREFIX=$1

set -ex
mkdir -p local/build
for d in gsl nlopt minuit hdf5; do
  if [ ! -e local/.built.${d} ]; then
    ./install-${d}.sh ${PREFIX}
  fi
done
