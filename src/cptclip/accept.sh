#!/bin/bash

source $2/accept-setup.sh

base="RdBu_10"

fixture="$TESTFIX/cpt/$base.cpt"

# clipping by z-range

assert_raises "./cptclip -o $base-cc.cpt -R 2.5/5 $fixture" 0
assert "equal-cpt $base-cc.cpt accept/$base-cc.cpt" true

# clipping by segment number

assert_raises "./cptclip -o $base-cd.cpt -s -R 2/5 $fixture" 0
assert "equal-cpt $base-cd.cpt accept/$base-cd.cpt" true

rm -f "$base-cc.cpt" "$base-cd.cpt"

source accept-teardown.sh
