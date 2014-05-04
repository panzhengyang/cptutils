#!/bin/bash

source $2/accept-setup.sh

base="subtle"
svg="$base.svg"

fixture="$TESTFIX/cpt/$base.cpt"

assert_raises "./cptsvg --geometry 500x100 -o $svg $fixture" 0
assert "svg-equal -b $svg accept/$svg" true

rm -f $svg

source accept-teardown.sh
