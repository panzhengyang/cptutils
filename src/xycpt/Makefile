# Makefile for xycpt
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.2 2004/03/18 02:26:05 jjg Exp $

OBJ      =  xycpt.o main.o options.o
RUBBISH += $(OBJ) xycpt

LDLIBS += 

default : all

all :

install :
	$(INSTALL_DATA) xycpt.1 $(mandir)/man1/xycpt.1
#	$(INSTALL_PROGRAM) xycpt $(bindir)/xycpt

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean
.SECONDARY : options.c options.h

xycpt : $(OBJ)

# for testing 

CPT = /usr/share/gmt/GMT_rainbow.cpt

test check : xycpt
	./xycpt --plain $(CPT)

include ../Common.mk
include ../Progs.mk



