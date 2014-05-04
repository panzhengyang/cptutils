#!/bin/bash

source $2/accept-setup.sh

base="Sunrise"
lut="$base.lut"

fixture_dir="$TESTFIX/ggr"

assert_raises \
    "export GIMP_GRADIENTS=$fixture_dir ; ./gimplut -o $lut $base" 0
assert "lut-equal -b $lut accept/$lut" true

rm -f $lut

source accept-teardown.sh
