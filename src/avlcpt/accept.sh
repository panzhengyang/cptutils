#!/bin/bash

source "$2/accept-setup.sh"

# 96004SP.avl is in the test directory, but does not
# contain a gradient

for base in ddvelev1a ddvelev9c
do
    fixture="$TESTFIX/avl/$base.avl"
    output="$base.cpt"
    reference="accept/$output"
    assert_raises "./avlcpt -n20/0/0 -v -o $output $fixture" 0
    assert "equal-cpt $output $reference" true
    rm $output
done

# this is an image file which, up to version 1.57, sent avlcpt
# into an infinite loop (in the flex scanner)

base='not-an-avl'
fixture="$TESTFIX/avl/$base.avl"
output="$base.cpt"
assert_raises "./avlcpt -n20/0/0 -v -o $output $fixture" 1
assert_raises "[ ! -e $output ]" 0
rm -f $output

source accept-teardown.sh
