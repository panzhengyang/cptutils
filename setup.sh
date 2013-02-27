#!/bin/sh
# script to build cptutils sources from a github checkout,
# of course you will need a fairly full set of development
# tools to to this (autoconf, gengetopt, gperf, xsltproc,..)

autoconf
autoheader
./configure
make version
make
make spotless
