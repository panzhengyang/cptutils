# Makefile for avlcpt
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.3 2005/06/27 17:51:16 jjg Exp $

OBJ      =  avlcpt.o main.o options.o avl.o
RUBBISH += $(OBJ) avlcpt

LDLIBS += 

default : all

all : avlcpt

install :
	$(INSTALL_DATA) avlcpt.1 $(mandir)/man1/avlcpt.1
	$(INSTALL_PROGRAM) avlcpt $(bindir)/avlcpt

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

avlcpt : $(OBJ)

# for testing 

RUBBISH += ddvelev9c.cpt

%.cpt : %.avl
	./avlcpt -v -o $*.cpt $< 

test check : avlcpt ddvelev9c.cpt

include ../Common.mk
include ../Progs.mk


