# Makefile for cpthsv
# (c) J.J.Green 2007
# $Id: Makefile,v 1.5 2005/06/27 17:50:27 jjg Exp $

NAME = cpthsv

OBJ      = cpthsv.o main.o options.o
RUBBISH += $(OBJ) $(NAME)

CFLAGS  += 
LDLIBS  +=

default : all

all : $(NAME)

install :
	$(INSTALL_PROGRAM) $(NAME) $(bindir)/$(NAME)
	$(INSTALL_DATA) $(NAME).1 $(mandir)/man1/$(NAME).1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

$(NAME) : $(OBJ)

# for testing 

BASE  = subtle
CPT   = $(BASE).cpt
SAT   = $(BASE)-sat.cpt
DESAT = $(BASE)-desat.cpt

$(SAT) : $(NAME) $(ORIG)
	./$(NAME) -o $(SAT) -T s90 -v $(CPT)

include ../Common.mk
include ../Progs.mk



