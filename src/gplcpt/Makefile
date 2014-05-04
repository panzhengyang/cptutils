# Makefile for gplcpt
#
# (c) J.J.Green 2010

NAME = gplcpt

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

include ../Common.mk

test check : accept
