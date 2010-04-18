# Makefile for cptclip
# (c) J.J.Green 2010
# $Id: Makefile,v 1.2 2010/04/11 22:37:23 jjg Exp jjg $

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

include ../Common.mk
include ../Progs.mk

# for testing 

BASE    = RdBu_10
CPT     = $(TESTD)/cpt/$(BASE).cpt
CPTCC   = $(BASE)-cc.cpt
CPTCD   = $(BASE)-cd.cpt
CPTTEST = $(CPTCC) $(CPTCD)

test : $(CPTTEST)

$(CPTCC) : $(NAME) $(CPT)
	./$(NAME) -o $(CPTCC) -v -R 2.5/5 $(CPT)

$(CPTCD) : $(NAME) $(CPT)
	./$(NAME) -o $(CPTCD) -v -s -R 2/5 $(CPT)

RUBBISH +=  $(CPTTEST)



