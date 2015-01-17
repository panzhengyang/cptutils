#!/bin/bash

source "$2/accept-setup.sh"

for base in subtle bad-whitespace
do
    svg="$base.svg"
    fixture="$TESTFIX/cpt/$base.cpt"

    assert_raises "./cptsvg --geometry 500x100 -o $svg $fixture" 0
    assert "equal-svg $svg accept/$svg" true

    rm -f $svg
done

source accept-teardown.sh
