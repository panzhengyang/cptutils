# Makefile for cptsvg
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.7 2012/01/22 19:41:57 jjg Exp jjg $

OBJ      = cptsvg.o main.o options.o
RUBBISH += $(OBJ) cptsvg

CFLAGS  += 
LDLIBS  += -lsvg

default : all

all : cptsvg cptsvg.1

install :
	$(INSTALL_PROGRAM) cptsvg $(DESTDIR)$(bindir)/cptsvg
	$(INSTALL_DATA) cptsvg.1 $(DESTDIR)$(mandir)/man1/cptsvg.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cptsvg : $(OBJ)

include ../Common.mk
include ../Progs.mk

# for testing 

BASE       = subtle
CPT        = $(TESTD)/cpt/$(BASE).cpt
SVG        = $(BASE).svg 

$(SVG) : cptsvg
	./cptsvg -o $(SVG) -v $(CPT)

check test : $(SVG)

RUBBISH += $(SVG)

