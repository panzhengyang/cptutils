# Makefile for cpttext
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.2 2004/03/18 02:26:05 jjg Exp $

OBJ      = main.o options.o cpttext.o
RUBBISH += $(OBJ) cpttext

LDLIBS += 

default : all

all : cpttext

install :
	$(INSTALL_PROGRAM) cpttext $(bindir)/cpttext
	$(INSTALL_DATA) cpttext.1 $(mandir)/man1/cpttext.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean
.SECONDARY : options.c options.h

cpttext : $(OBJ)

# for testing 

CPT = /usr/share/gmt/GMT_cool.cpt

test check : cpttext
	./cpttext -t "big dog's arse" -v $(CPT)

include ../Common.mk
include ../Progs.mk



