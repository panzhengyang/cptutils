#!/bin/bash

source $2/accept-setup.sh

base="Sunrise"
lut="$base.lut"
ggr="$base.ggr"

fixtures="$TESTFIX/ggr"

assert_raises "./gimplut -o $lut $fixtures/$ggr" 0
assert "equal-lut $lut accept/$lut" true

rm -f $lut

source accept-teardown.sh
