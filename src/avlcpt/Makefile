# Makefile for avlcpt
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.12 2012/03/09 21:55:19 jjg Exp jjg $

OBJ      =  avlcpt.o main.o options.o avlgrad.o
RUBBISH += $(OBJ) avlcpt

LDLIBS  += -lcpt -lfill -lodb -lident

default : all

all : avlcpt avlcpt.1

install :
	$(INSTALL_DATA) avlcpt.1 $(DESTDIR)$(mandir)/man1/avlcpt.1
	$(INSTALL_PROGRAM) avlcpt $(DESTDIR)$(bindir)/avlcpt

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

# for testing (96004SP.avl is in the test directory
# but fails, it contains no gradient) 

TESTBASE = ddvelev1a ddvelev9c 

RUBBISH += $(CPT)

cpt : avlcpt
	for base in $(TESTBASE) ; do \
	  ./avlcpt -b0/0/20 -f0/20/0 -n20/0/0 -v -o $$base.cpt \
	  	$(TESTD)/avl/$$base.avl ; \
	done

cpt-clean : 
	for base in $(TESTBASE) ; do \
	  $(RM) $$base.cpt ; \
	done

test check : cpt


