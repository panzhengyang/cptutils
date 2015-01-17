#!/bin/bash

source "$2/accept-setup.sh"

base1=tpsfhm
base2=pakistan

fix1="$TESTFIX/cpt/$base1.cpt"
fix2="$TESTFIX/cpt/$base2.cpt"

out12="$base1-$base2.cpt"
out21="$base2-$base1.cpt"

reference="accept/$out21"

assert_raises "./cptcat -o $out12 $fix1 $fix2" 0
assert_raises "./cptcat -o $out21 $fix2 $fix1" 0
assert "equal-cpt $out12 $out21" true
assert "equal-cpt $out12 $reference" true

rm -f $out12 $out21

source accept-teardown.sh
