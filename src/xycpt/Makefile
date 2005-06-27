# Makefile for xycpt
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.2 2004/04/12 00:39:00 jjg Exp jjg $

OBJ      =  xycpt.o main.o options.o
RUBBISH += $(OBJ) xycpt

LDLIBS += 

default : all

all : xycpt

install :
	$(INSTALL_DATA) xycpt.1 $(mandir)/man1/xycpt.1
	$(INSTALL_PROGRAM) xycpt $(bindir)/xycpt

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

xycpt : $(OBJ)

# for testing 

TESTCPT  = test.1.cpt test.2.cpt test.3.cpt test.4.cpt
RUBBISH += $(TESTCPT)

test check : xycpt
	$(MAKE) $(TESTCPT)

%.cpt : %.dat 
	./xycpt -v -o $*.cpt $< 

include ../Common.mk
include ../Progs.mk



