# Makefile for cptsvg
# (c) J.J.Green 2001,2004

NAME = cptsvg

OBJ      = cptsvg.o main.o options.o
RUBBISH += $(OBJ) cptsvg

CFLAGS  += 
LDLIBS  += -lcpt -lfill -lsvg -lbt

default : all

all : cptsvg cptsvg.1

install :
	$(INSTALL_PROGRAM) cptsvg $(DESTDIR)$(bindir)/cptsvg
	$(INSTALL_DATA) cptsvg.1 $(DESTDIR)$(mandir)/man1/cptsvg.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cptsvg : $(OBJ)

include ../Common.mk

check test : accept

