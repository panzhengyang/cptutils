#!/bin/bash

source $2/accept-setup.sh

base="GMT_haxby"
cpt="$base.cpt"
css1="$base-01.css"
css2="$base-02.css"

fixture="$TESTFIX/cpt/$cpt"

# default 

assert_raises "./cptcss $fixture > $css1" 0
assert "equal-css $css1 accept/$css1" true

# custom label

assert_raises "./cptcss -f 'level-%.2' $fixture > $css2" 0
assert "equal-css $css2 accept/$css2" true

rm -f $css1 $css2

source accept-teardown.sh
