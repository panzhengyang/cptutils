#!/bin/bash

source "$2/accept-setup.sh"

fixtures="$TESTFIX/ggr"

for base in "Sunrise" "mars"
do
    svg="$base.svg"
    ggr="$base.ggr"

    assert_raises "./gimpsvg -p -o $svg $fixtures/$ggr" 0
    assert "equal-svg $svg accept/$svg" true

    rm -f $svg
done

source accept-teardown.sh
