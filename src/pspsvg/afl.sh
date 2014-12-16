#!/bin/sh

TCDIR="afl/testcase"
FDDIR="afl/findings"
FXDIR="../fixtures/grd3"

mkdir -p $TCDIR
mkdir -p $FDDIR
for file in accented.jgd test.1.jgd test.2.jgd test.3.jgd test.4.jgd
do
    cp $FXDIR/$file $TCDIR
done

AFL_SKIP_CPUFREQ=true afl-fuzz -d -i $TCDIR -o $FDDIR ./pspsvg @@
