# Makefile for cptgimp
# (c) J.J.Green 2001,2004

NAME = cptinfo

OBJ      = main.o options.o cptinfo.o
RUBBISH += $(OBJ) cptinfo

LDLIBS += -lcpt -lfill

default : all

all : cptinfo cptinfo.1

install :
	$(INSTALL_PROGRAM) cptinfo $(DESTDIR)$(bindir)/cptinfo
	$(INSTALL_DATA) cptinfo.1 $(DESTDIR)$(mandir)/man1/cptinfo.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cptinfo : $(OBJ)

include ../Common.mk

test check : accept




