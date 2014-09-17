#!/bin/sh

# run scan-build (clang) static analysis and dump the
# results in the parent-directory

SCAN_RESULTS="../clang-static"

scan-build -v ./configure
scan-build -v -o $SCAN_RESULTS make
