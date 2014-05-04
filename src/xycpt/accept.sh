#!/bin/bash

source $2/accept-setup.sh

fixtures="$TESTFIX/xy"

for n in `seq 1 4` 
do
    base="test-i$n" 
    cpt="$base.cpt"
    dat="$base.dat"
    assert_raises "./xycpt -o $cpt $fixtures/$dat" 0
    assert "cpt-equal -b $cpt accept/$cpt" true
    rm -f $cpt

    base="test-u$n" 
    cpt="$base.cpt"
    dat="$base.dat"
    assert_raises "./xycpt -u -o $cpt $fixtures/$dat" 0
    assert "cpt-equal -b $cpt accept/$cpt" true
    rm -f $cpt
done

source accept-teardown.sh
