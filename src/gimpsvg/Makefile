# Makefile for gimpcpt
# (c) J.J.Green 2001
# $Id: Makefile,v 1.1 2004/02/09 01:34:28 jjg Exp jjg $

OBJ      = gimpcpt.o main.o findgrad.o
RUBBISH  = $(OBJ) gimpcpt

default : all

all : gimpcpt

install :
	$(INSTALL_PROGRAM) gimpcpt $(bindir)/gimpcpt
	$(INSTALL_DATA) gimpcpt.1 $(mandir)/man1/gimpcpt.1

test: $(TESTGRAD).ps
	gv $(TESTGRAD).ps

clean :
	$(RM) $(RUBBISH)

veryclean : 

gimpcpt : $(OBJ) 

.PHONEY : all test doc

include ../Common.mk
include ../Progs.mk



