# Makefile for cptcss
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.5 2012/09/03 16:12:39 jjg Exp jjg $

OBJ      = main.o options.o cptcss.o
RUBBISH += $(OBJ) cptcss

LDLIBS += 

default : all

all : cptcss cptcss.1

install :
	$(INSTALL_PROGRAM) cptcss $(DESTDIR)$(bindir)/cptcss
	$(INSTALL_DATA) cptcss.1 $(DESTDIR)$(mandir)/man1/cptcss.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cptcss : $(OBJ)

# for testing 

CPT = $(TESTD)/cpt/GMT_haxby.cpt

test check : cptcss
	./cptcss -f "level-%.2i" -v $(CPT)
	./cptcss -v $(CPT)

include ../Progs.mk
include ../Common.mk


