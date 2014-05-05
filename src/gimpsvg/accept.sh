#!/bin/bash

source $2/accept-setup.sh

base="Sunrise"
svg="$base.svg"

fixtures="$TESTFIX/ggr"

assert_raises \
    "export GIMP_GRADIENTS=$fixtures ; ./gimpsvg -p -o $svg $base" 0
assert "equal-svg $svg accept/$svg" true

rm -f $svg

source accept-teardown.sh
