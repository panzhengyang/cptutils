#!/bin/sh

TCDIR="afl/testcase"
FDDIR="afl/findings"
FXDIR="../fixtures/cpt"

mkdir -p $TCDIR
mkdir -p $FDDIR
cp $FXDIR/test.cpt $TCDIR
AFL_SKIP_CPUFREQ=true afl-fuzz -d -i $TCDIR -o $FDDIR ./cptsvg @@
