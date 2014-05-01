set -e

NAME=$1
TESTLIB=$2

export PATH=$TESTLIB:$PATH
source assert.sh

