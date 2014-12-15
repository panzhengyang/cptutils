#!/bin/sh

TCDIR="afl/testcase"
FDDIR="afl/findings"
FXDIR="../fixtures/avl"

mkdir -p $TCDIR
mkdir -p $FDDIR
cp $FXDIR/ddvelev9c.avl $TCDIR
AFL_SKIP_CPUFREQ=true afl-fuzz -d -i $TCDIR -o $FDDIR ./avlcpt @@
