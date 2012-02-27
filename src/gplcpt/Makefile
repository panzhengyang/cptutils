# Makefile for gplcpt
#
# (c) J.J.Green 2010
# $Id: Makefile,v 1.2 2012/01/22 19:42:02 jjg Exp jjg $

OBJ      =  gplcpt.o main.o options.o
RUBBISH += $(OBJ) gplcpt

LDLIBS += 

default : all

all : gplcpt

install :
	$(INSTALL_DATA) gplcpt.1 $(DESTDIR)$(mandir)/man1/gplcpt.1
	$(INSTALL_PROGRAM) gplcpt $(DESTDIR)$(bindir)/gplcpt

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

gplcpt : $(OBJ)

# for testing 

GRAD     = Caramel
RUBBISH += $(GRAD).cpt

test check : $(GRAD).cpt

$(GRAD).cpt : gplcpt 
	./gplcpt -v -o $(GRAD).cpt $(TESTD)/gpl/$(GRAD).gpl 

include ../Common.mk
include ../Progs.mk



