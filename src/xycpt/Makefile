# Makefile for xycpt
#
# (c) J.J.Green 2001,2004

NAME = xycpt

OBJ      =  xycpt.o main.o options.o
RUBBISH += $(OBJ) xycpt

LDLIBS += -lcpt -lfill

default : all

all : xycpt xycpt.1

install :
	$(INSTALL_DATA) xycpt.1 $(DESTDIR)$(mandir)/man1/xycpt.1
	$(INSTALL_PROGRAM) xycpt $(DESTDIR)$(bindir)/xycpt

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

xycpt : $(OBJ)

include ../Common.mk

test check : accept
