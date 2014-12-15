#!/bin/sh

TCDIR="afl/testcase"
FDDIR="afl/findings"
FXDIR="../fixtures/cpt"

mkdir -p $TCDIR
mkdir -p $FDDIR
for file in blue.cpt GMT_haxby.cpt pakistan.cpt subtle.cpt tpsfhm.cpt
do
    cp $FXDIR/$file $TCDIR
done
AFL_SKIP_CPUFREQ=true afl-fuzz -d -i $TCDIR -o $FDDIR ./cptsvg @@
