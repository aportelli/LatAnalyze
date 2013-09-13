#!/usr/bin/env bash

echo "extracting Eigen headers..."
./update_eigen.sh eigen-eigen-*.tar.bz2
rm -rf .buildutils
mkdir -p .buildutils/m4
autoreconf -fvi
