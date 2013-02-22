# Makefile for gplcpt
#
# (c) J.J.Green 2010
# $Id: Makefile,v 1.6 2012/10/15 22:05:00 jjg Exp $

OBJ      =  gplcpt.o main.o options.o
RUBBISH += $(OBJ) gplcpt

LDLIBS += -lcpt -lfill

default : all

all : gplcpt gplcpt.1

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


