#!/usr/bin/env bash

set -ex
mkdir -p local/build
for d in nlopt minuit hdf5; do
  if [ ! -e local/.built.${d} ]; then
    ./install-${d}.sh
  fi
done
./install-latcore.sh
