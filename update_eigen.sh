#!/usr/bin/env bash

if (( $# != 1 )); then
    echo "usage: `basename $0` <archive>" 1>&2
fi
ARC=$1

INITDIR=`pwd`
ARCDIR=`tar -tf ${ARC} | head -n1 | sed -e 's@/.*@@'`
tar -xf ${ARC}
cd ${ARCDIR}
rm -rf latan/Eigen
tar -cf - Eigen --exclude='*.txt' | tar -xvf - -C ../latan/
cd ${INITDIR}
rm -rf ${ARCDIR}
