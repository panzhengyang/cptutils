# Makefile for cptclip
# (c) J.J.Green 2010
# $Id: Makefile,v 1.7 2012/10/15 21:46:55 jjg Exp $

NAME = cptclip

OBJ      = $(NAME).o main.o options.o
RUBBISH += $(OBJ) $(NAME)

CFLAGS  += 
LDLIBS  += -lcpt -lfill

default : all

all : $(NAME) $(NAME).1

install :
	$(INSTALL_PROGRAM) $(NAME) $(DESTDIR)$(bindir)/$(NAME)
	$(INSTALL_DATA) $(NAME).1 $(DESTDIR)$(mandir)/man1/$(NAME).1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

$(NAME) : $(OBJ)

include ../Common.mk

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



