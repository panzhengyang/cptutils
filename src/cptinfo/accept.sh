#!/bin/bash

source $2/accept-setup.sh

base="GMT_gebco"
info_plain="$base-plain.info"
info_csv="$base-csv.info"

fixture="$TESTFIX/cpt/$base.cpt"

# plain output

assert_raises "./cptinfo -o $info_plain --plain $fixture" 0
assert "diff $info_plain accept/$info_plain | wc -l" 0

# csv output

assert_raises "./cptinfo -o $info_csv --csv $fixture" 0
assert "diff $info_csv accept/$info_csv | wc -l" 0

rm -f $info_plain $info_csv

source accept-teardown.sh
