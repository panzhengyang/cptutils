#!/bin/bash

source "$2/accept-setup.sh"

base="Sunrise"
svg="$base.svg"
ggr="$base.ggr"

fixtures="$TESTFIX/ggr"

assert_raises "./gimpsvg -p -o $svg $fixtures/$ggr" 0
assert "equal-svg $svg accept/$svg" true

rm -f $svg

source accept-teardown.sh
