#!/bin/sh
# build for testing with American fuzzy lop 
# http://lcamtuf.coredump.cx/afl/

CC=afl-gcc ./configure
make clean all
