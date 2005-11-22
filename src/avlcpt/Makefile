# Makefile for avlcpt
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.5 2005/11/19 17:16:08 jjg Exp jjg $

OBJ      =  avlcpt.o main.o options.o avlgrad.o
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

.SECONDARY : odbscan.c odbscan.h odbparse.c

odbscan.c odbscan.h : odbscan.l 
	flex odbscan.l 

odbparse.c : odbparse.y 
	bison odbparse.y

# for testing 

CPT = avl/ddvelev1a.cpt avl/ddvelev9c.cpt avl/96004SP.cpt 

RUBBISH += $(CPT)

%.cpt : %.avl
	./avlcpt -v -o $*.cpt $< 

$(CPT) : avlcpt

test check : $(CPT)

include ../Common.mk
include ../Progs.mk


