#!/bin/bash

source "$2/accept-setup.sh"

fixture_dir="$TESTFIX/grd3"

for n in `seq 1 4` 
do
    base="test.$n" 
    jgd="$base.jgd"
    svg="$base.svg"
    assert_raises "./pspsvg -p -o $svg $fixture_dir/$jgd" 0
    assert "equal-svg $svg accept/$svg" true
    rm -f $svg
done

source accept-teardown.sh
