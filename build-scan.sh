#!/bin/sh

# run scan-build (clang) static analysis and dump the
# results in the $SCAN_RESULTS directory

SCAN_RESULTS="/tmp/scan-build"

if [ -e Makefile ]
then
    make spotless
fi
scan-build -v ./configure
scan-build -v -o $SCAN_RESULTS make
