#!/usr/bin/env bash

rm -rf .buildutils
mkdir -p .buildutils/m4
./update_eigen.sh eigen-3.3.4.tar.bz2
autoreconf -fvi
