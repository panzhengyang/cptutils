# Makefile for gimpcpt
# (c) J.J.Green 2001
# $Id: Makefile.in,v 1.2 2002/07/23 00:09:53 jjg Exp $

OBJ      = gimpcpt.o main.o findgrad.o
RUBBISH  = $(OBJ) gimpcpt
CFLAGS  += -Wall -I../common/
LDFLAGS += -L../common/ 
LDLIBS  += -lgimpcpt

default : all

all : gimpcpt

install :
	$(INSTALL_PROGRAM) gimpcpt $(bindir)/gimpcpt
	$(INSTALL_DATA) gimpcpt.1 $(mandir)/man1/gimpcpt.1

test: $(TESTGRAD).ps
	gv $(TESTGRAD).ps

clean :
	$(RM) $(RUBBISH)

veryclean : 

gimpcpt : $(OBJ) 

.PHONEY : all test doc

include ../Common.mk




