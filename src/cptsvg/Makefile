# Makefile for cptsvg
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.5 2005/06/27 17:50:27 jjg Exp jjg $

OBJ      = cptsvg.o main.o options.o
RUBBISH += $(OBJ) cptsvg

CFLAGS  += 
LDLIBS  += -lsvg

default : all

all : cptsvg

install :
	$(INSTALL_PROGRAM) cptsvg $(bindir)/cptsvg
	$(INSTALL_DATA) cptsvg.1 $(mandir)/man1/cptsvg.1

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

