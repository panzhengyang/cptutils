#!/bin/bash

source $2/accept-header.sh

assert "echo test" "test"
assert "echo test" "test"
assert "echo test" "test"

source accept-footer.sh
