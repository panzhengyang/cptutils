#!/bin/bash

source $2/accept-setup.sh

fixtures="$TESTFIX/grd5"

bases=$(cat <<EOF
my-custom-gradient-3-rgb
my-custom-gradient-3-hsb
my-custom-gradient-3-lab
neverhappens-titi-montoya
EOF
)

for base in $bases 
do
    grd="$base.grd"
    svg="$base.svg"
    assert_raises "./pssvg -o $svg $fixtures/$grd" 0
    assert "equal-svg $svg accept/$svg" true
    rm -f $svg
done

# pssgv 1.53 segfaulted when passed this file (which is
# a grd3 file rather than a grd5 one), rather than 
# exiting gracefully

assert_raises "./pssvg -o not_exist.svg $fixtures/ES_Coffe.grd" 1

# segfaults from hostile input files (found with AFL)

for id in `seq -w 0 12`
do
    grd="afl-2014-12-15-$id"
    assert_raises "./pssvg $fixtures/$grd" 1
done

source accept-teardown.sh
