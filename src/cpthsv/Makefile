# Makefile for cpthsv
# (c) J.J.Green 2007
# $Id: Makefile,v 1.2 2010/04/18 15:30:21 jjg Exp jjg $

NAME = cpthsv

OBJ      = cpthsv.o main.o options.o
RUBBISH += $(OBJ) $(NAME)

CFLAGS  += 
LDLIBS  +=

default : all

all : $(NAME)

install :
	$(INSTALL_PROGRAM) $(NAME) $(DESTDIR)$(bindir)/$(NAME)
	$(INSTALL_DATA) $(NAME).1 $(DESTDIR)$(mandir)/man1/$(NAME).1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

$(NAME) : $(OBJ)

include ../Common.mk
include ../Progs.mk

# for testing 

BASE  = subtle
CPT   = $(TESTD)/cpt/$(BASE).cpt
SAT   = $(BASE)-sat.cpt
DESAT = $(BASE)-desat.cpt

test : $(SAT)

$(SAT) : $(NAME) $(ORIG)
	./$(NAME) -o $(SAT) -T s90 -v $(CPT)

RUBBISH += $(SAT)
