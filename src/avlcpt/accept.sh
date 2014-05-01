#!/bin/bash

source $2/accept-header.sh

# 96004SP.avl is in the test directory, but does not
# contain a gradient

for base in ddvelev1a ddvelev9c
do
    fixture="$TESTFIX/avl/$base.avl"
    output="$base.cpt"
    reference="accept/$output"
    assert_raises "./avlcpt -n20/0/0 -v -o $output $fixture" 0
    assert "diff -I cptutils $output $reference"
    rm $output
done

source accept-footer.sh
