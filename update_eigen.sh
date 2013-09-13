#!/usr/bin/env bash

if (( $# != 1 )); then
    echo "usage: `basename $0` <archive>" 1>&2
fi
ARC=$1

INITDIR=`pwd`
rm -rf latan/Eigen
ARCDIR=`tar -tf ${ARC} | head -n1 | sed -e 's@/.*@@'`
tar -xf ${ARC}
cd ${ARCDIR}
tar -cf - Eigen --exclude='*.txt' | tar -xf - -C ../latan/
cd ../latan
echo 'eigen_files =\' > eigen_files.mk
find Eigen -type f -print | sed 's/^/  /;$q;s/$/ \\/' >> eigen_files.mk
cd ${INITDIR}
rm -rf ${ARCDIR}
