# Makefile for cptgimp
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.1 2004/02/29 20:04:54 jjg Exp jjg $

OBJ      = main.o options.o cptinfo.o
RUBBISH += $(OBJ) cptinfo

LDLIBS += 

default : all

all : cptinfo

install :
	$(INSTALL_PROGRAM) cptinfo $(bindir)/cptinfo
	$(INSTALL_DATA) cptinfo.1 $(mandir)/man1/cptinfo.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean
.SECONDARY : options.c options.h

cptinfo : $(OBJ)

# for testing 

CPT = /usr/share/gmt/GMT_rainbow.cpt

test check : cptinfo
	./cptinfo --plain $(CPT)

include ../Common.mk
include ../Progs.mk



