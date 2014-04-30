#!/bin/bash

set -e
. assert.sh

assert "echo test" "test"
assert_end examples