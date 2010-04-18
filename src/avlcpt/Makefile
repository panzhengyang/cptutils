# Makefile for avlcpt
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.7 2005/12/04 18:25:53 jjg Exp jjg $

OBJ      =  avlcpt.o main.o options.o avlgrad.o
RUBBISH += $(OBJ) avlcpt

LDLIBS += 

default : all

all : avlcpt

install :
	$(INSTALL_DATA) avlcpt.1 $(mandir)/man1/avlcpt.1
	$(INSTALL_PROGRAM) avlcpt $(bindir)/avlcpt

clean : cpt-clean
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

avlcpt : $(OBJ)

.SECONDARY : odbscan.c odbscan.h odbparse.c

odbscan.c odbscan.h : odbscan.l 
	flex odbscan.l 

odbparse.c : odbparse.y 
	bison odbparse.y

include ../Common.mk
include ../Progs.mk

# for testing 

TESTBASE = ddvelev1a ddvelev9c 96004SP 

RUBBISH += $(CPT)

cpt : avlcpt
	for base in $(TESTBASE) ; do \
	  ./avlcpt -b0/0/20 -f0/20/0 -n20/0/0 -v -o $$base.cpt \
	  	$(TESTD)/avl/$$base.avl ; \
	done

cpt-clean : 
	for base in $(TESTBASE) ; do \
	  $(RM)/$$base.cpt ; \
	done

test check : cpt


