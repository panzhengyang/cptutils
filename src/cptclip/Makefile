# Makefile for cptcont
# (c) J.J.Green 2010
# $Id: Makefile,v 1.1 2010/04/04 17:27:23 jjg Exp $

NAME = cptclip

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
CPTCLP  = $(BASE)-clip.cpt

test : $(CPTCLP) 

$(CPTCLP) : $(NAME) $(CPT)
	./$(NAME) -o $(CPTLP) -v 2 5 $(CPT)

RUBBISH += $(CPTCLP)

include ../Common.mk
include ../Progs.mk



