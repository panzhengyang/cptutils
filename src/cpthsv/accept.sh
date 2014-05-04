#!/bin/bash

source $2/accept-setup.sh

base="subtle"

fixture="$TESTFIX/cpt/$base.cpt"
cpt_sat="$base-sat.cpt"

# saturation

assert_raises "./cpthsv -o $cpt_sat -T s90,v110 $fixture" 0
assert "cpt-equal -b $cpt_sat accept/$cpt_sat" true

rm -f $cpt_sat

source accept-teardown.sh
