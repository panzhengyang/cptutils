# Makefile for cptcont
# (c) J.J.Green 2010
# $Id: Makefile,v 1.1 2010/04/04 17:27:23 jjg Exp jjg $

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

include ../Common.mk
include ../Progs.mk

# for testing 

BASE    = RdBu_10
CPT     = $(TESTD)/cpt/$(BASE).cpt
CPTCONT = $(BASE)-cont.cpt
CPTAC   = $(BASE)-ac.cpt
CPTTEST = $(CPTCONT) $(CPTAC) 

test : $(CPTTEST)

$(CPTCONT) : $(NAME) $(CPT)
	./$(NAME) -o $(CPTCONT) -v $(CPT)

$(CPTAC) : $(NAME) $(CPT)
	./$(NAME) -o $(CPTAC) -v -p 60 $(CPT)

RUBBISH += $(CPTCONT) $(CPTTEST)



