# Makefile for avlcpt
#
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.3 2005/11/15 00:45:31 jjg Exp jjg $

OBJ      =  avlcpt.o main.o options.o avl.o odb_bridge.o odb.o ident.o identtab.o odbscan.o odbparse.o
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

RUBBISH += ddvelev9c.cpt

%.cpt : %.avl
	./avlcpt -v -o $*.cpt $< 

ddvelev9c.cpt : avlcpt

test check : ddvelev9c.cpt

include ../Common.mk
include ../Progs.mk


