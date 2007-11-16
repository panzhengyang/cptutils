# Makefile for cptcss
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.1 2005/07/19 22:18:11 jjg Exp jjg $

OBJ      = main.o options.o cptcss.o
RUBBISH += $(OBJ) cptcss

LDLIBS += 

default : all

all : cptcss

install :
	$(INSTALL_PROGRAM) cptcss $(bindir)/cptcss
	$(INSTALL_DATA) cptcss.1 $(mandir)/man1/cptcss.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cptcss : $(OBJ)

# for testing 

CPT = /usr/share/gmt/cpt/GMT_haxby.cpt

test check : cptcss
	./cptcss -f "level-%.2i" -v $(CPT)
	./cptcss -v $(CPT)

include ../Common.mk
include ../Progs.mk



