# Makefile for gplcpt
#
# (c) J.J.Green 2010
# $Id: Makefile,v 1.3 2005/06/27 17:51:52 jjg Exp $

OBJ      =  gplcpt.o main.o options.o
RUBBISH += $(OBJ) gplcpt

LDLIBS += 

default : all

all : gplcpt

install :
	$(INSTALL_DATA) gplcpt.1 $(mandir)/man1/gplcpt.1
	$(INSTALL_PROGRAM) gplcpt $(bindir)/gplcpt

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

gplcpt : $(OBJ)

# for testing 

TESTCPT  = test.1.cpt test.2.cpt test.3.cpt test.4.cpt
RUBBISH += $(TESTCPT)

test check : gplcpt
	$(MAKE) $(TESTCPT)

%.cpt : %.dat 
	./gplcpt -v -o $*.cpt $< 

include ../Common.mk
include ../Progs.mk



