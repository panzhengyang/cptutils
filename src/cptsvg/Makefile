# Makefile for cptsvg
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.4 2005/06/05 13:09:51 jjg Exp jjg $

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

# for testing 

BASE       = subtle
CPT        = $(BASE).cpt
SVG        = $(BASE).svg 
RUBBISH   += $(SVG)
XMLPRETTY  = xmlpretty --PrettyWhiteNewline --PrettyWhiteIndent --CatchEmptyElement

$(SVG) : cptsvg
	./cptsvg -o $(SVG) -v $(CPT)

check test : $(SVG)
	cat $(SVG) | $(XMLPRETTY)

include ../Common.mk
include ../Progs.mk



