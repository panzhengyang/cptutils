# Makefile for cptcont
# (c) J.J.Green 2010
# $Id: Makefile,v 1.1 2007/11/16 17:06:30 jjg Exp $

NAME = cptcont

OBJ      = $(NAME).o main.o options.o
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

BASE    = RdBu_10
CPT     = $(BASE).cpt
CPTCONT = $(BASE)-cont.cpt
CPTAC   = $(BASE)-ac.cpt

test : $(CPTCONT) 

$(CPTCONT) : $(NAME) $(CPT)
	./$(NAME) -o $(CPTCONT) -v $(CPT)

RUBBISH += $(CPTCONT) $(CPTAC)

include ../Common.mk
include ../Progs.mk



