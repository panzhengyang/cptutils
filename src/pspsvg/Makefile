# Makefile for pspsvg
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.3 2012/02/27 18:59:57 jjg Exp jjg $

OBJ      =  pspsvg.o main.o options.o gstack.o
RUBBISH += $(OBJ) pspsvg

CFLAGS += 
LDLIBS += 

default : all

all : pspsvg pspsvg.1

install :
	$(INSTALL_DATA) pspsvg.1 $(DESTDIR)$(mandir)/man1/pspsvg.1
	$(INSTALL_PROGRAM) pspsvg $(DESTDIR)$(bindir)/pspsvg

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
	  ./pspsvg -p -v -o t$$n.svg \
	              $(TESTD)/grd/test.$$n.jgd ; \
	done

test check : $(TESTSVG)


