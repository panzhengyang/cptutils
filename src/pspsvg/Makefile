# Makefile for pspsvg
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.6 2010/04/18 16:02:44 jjg Exp $

OBJ      =  pspsvg.o main.o options.o gstack.o
RUBBISH += $(OBJ) pspsvg

CFLAGS += 
LDLIBS += 

default : all

all : pspsvg

install :
	$(INSTALL_DATA) pspsvg.1 $(mandir)/man1/pspsvg.1
	$(INSTALL_PROGRAM) pspsvg $(bindir)/pspsvg

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

pspsvg : $(OBJ)

include ../Common.mk
include ../Progs.mk


TESTSVG  = t1.svg t2.svg t3.svg t4.svg
RUBBISH += $(TESTSVG)

$(TESTSVG) : pspsvg
	for n in `seq 1 4` ; do \
	  ./pspsvg -v -o t$$n.svg \
	              $(TESTD)/grd/test.$$n.jgd ; \
	done

test check : $(TESTSVG)


