# Makefile for cptgimp
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.6 2012/02/27 19:01:34 jjg Exp jjg $

OBJ      = main.o options.o cptinfo.o
RUBBISH += $(OBJ) cptinfo

LDLIBS += 

default : all

all : cptinfo cptinfo.1

install :
	$(INSTALL_PROGRAM) cptinfo $(DESTDIR)$(bindir)/cptinfo
	$(INSTALL_DATA) cptinfo.1 $(DESTDIR)$(mandir)/man1/cptinfo.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cptinfo : $(OBJ)

# for testing 

CPT = $(TESTD)/cpt/GMT_gebco.cpt

test check : cptinfo
	./cptinfo --plain $(CPT)
	./cptinfo --csv $(CPT)

include ../Progs.mk
include ../Common.mk


