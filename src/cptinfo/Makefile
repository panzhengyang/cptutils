# Makefile for cptgimp
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.8 2012/10/15 21:54:32 jjg Exp $

OBJ      = main.o options.o cptinfo.o
RUBBISH += $(OBJ) cptinfo

LDLIBS += -lcpt -lfill

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

include ../Common.mk


