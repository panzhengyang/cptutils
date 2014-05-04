# Makefile for cptcss
# (c) J.J.Green 2001,2004

NAME = cptcss

OBJ      = main.o options.o cptcss.o
RUBBISH += $(OBJ) cptcss

LDLIBS += -lcpt -lfill

default : all

all : cptcss cptcss.1

install :
	$(INSTALL_PROGRAM) cptcss $(DESTDIR)$(bindir)/cptcss
	$(INSTALL_DATA) cptcss.1 $(DESTDIR)$(mandir)/man1/cptcss.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cptcss : $(OBJ)

include ../Common.mk

test check : accept




