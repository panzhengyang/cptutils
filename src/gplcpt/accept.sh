#!/bin/bash

source $2/accept-setup.sh

base="Caramel"
cpt="$base.cpt"
fixture="$TESTFIX/gpl/$base.gpl"

assert_raises "./gplcpt -o $cpt $fixture" 0
assert "cpt-equal -b $cpt accept/$cpt" true

rm -f $cpt

source accept-teardown.sh
