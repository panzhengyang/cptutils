#!/bin/sh

TCDIR="afl/testcase"
FDDIR="afl/findings"
FXDIR="../fixtures/ggr"

mkdir -p $TCDIR
mkdir -p $FDDIR
cp $FXDIR/Sunrise.ggr $TCDIR
AFL_SKIP_CPUFREQ=true afl-fuzz -d -i $TCDIR -o $FDDIR ./gimplut @@
