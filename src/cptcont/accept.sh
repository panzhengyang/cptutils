#!/bin/bash

source $2/accept-setup.sh

base="RdBu_10"
cpt_cont="$base-cont.cpt"
cpt_ac="$base-ac.cpt"

fixture="$TESTFIX/cpt/$base.cpt"

# continuous cpt

assert_raises "./cptcont -o $cpt_cont -m $fixture" 0
assert "equal-cpt $cpt_cont accept/$cpt_cont" true

# almost continuous

assert_raises "./cptcont -o $cpt_ac -m -p 60 $fixture" 0
assert "equal-cpt $cpt_ac accept/$cpt_ac" true

rm -f $cpt_cont $cpt_ac

source accept-teardown.sh
