#!/bin/sh

TCDIR="afl/testcase"
FDDIR="afl/findings"
FXDIR="../fixtures/grd5"

mkdir -p $TCDIR
mkdir -p $FDDIR
for file in my-custom-gradient-3-rgb.grd
do
    cp $FXDIR/$file $TCDIR
done

AFL_SKIP_CPUFREQ=true afl-fuzz -d -i $TCDIR -o $FDDIR ./pssvg @@
