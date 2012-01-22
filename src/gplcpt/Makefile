# Makefile for gplcpt
#
# (c) J.J.Green 2010
# $Id: Makefile,v 1.1 2009/12/28 16:44:35 jjg Exp jjg $

OBJ      =  gplcpt.o main.o options.o
RUBBISH += $(OBJ) gplcpt

LDLIBS += 

default : all

all : gplcpt

install :
	$(INSTALL_DATA) gplcpt.1 $(DESTDIR)$(mandir)/man1/gplcpt.1
	$(INSTALL_PROGRAM) gplcpt $(DESTDIR)$(bindir)/gplcpt

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



