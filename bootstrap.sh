#!/usr/bin/env bash

rm -rf .buildutils
mkdir -p .buildutils/m4
autoreconf -fvi
