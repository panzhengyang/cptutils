#!/bin/bash

source "$2/accept-setup.sh"

fixtures="$TESTFIX/ggr"

for base in "Sunrise" "mars"
do
    lut="$base.lut"
    ggr="$base.ggr"

    assert_raises "./gimplut -o $lut $fixtures/$ggr" 0
    assert "equal-lut $lut accept/$lut" true

    rm -f $lut
done

source accept-teardown.sh
