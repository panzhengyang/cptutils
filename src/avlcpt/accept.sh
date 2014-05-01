#!/bin/bash

set -e

TESTLIB=$1
NAME='avlcpt'
. $TESTLIB/assert.sh

assert "echo test" "test"

assert_end $NAME
