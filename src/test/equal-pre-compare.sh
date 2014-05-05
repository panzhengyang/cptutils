#!/bin/bash
# equal-* helper

file1=$1
file2=$2

# first arg must exist

if [ ! -e $file1 ]
then
    echo "file not found: $file1"
    exit 1
fi

# create second (a copy of the first) if not exist

if [ ! -e $file2 ]
then
    echo "creating reference file: $file2"
    cp $file1 $file2
fi


