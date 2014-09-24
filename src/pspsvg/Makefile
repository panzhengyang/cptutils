# Makefile for pspsvg
#
# (c) J.J.Green 2001, 2004

NAME = pspsvg

OBJ      =  pspsvg.o main.o options.o
RUBBISH += $(OBJ) pspsvg

CFLAGS += 
LDLIBS += -lsvg -lgrdx -lgrd3 -lgstk -lfill -lbt

default : all

all : pspsvg pspsvg.1

install :
	$(INSTALL_DATA) pspsvg.1 $(DESTDIR)$(mandir)/man1/pspsvg.1
	$(INSTALL_PROGRAM) pspsvg $(DESTDIR)$(bindir)/pspsvg

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

pspsvg : $(OBJ)

include ../Common.mk

test check : accept


