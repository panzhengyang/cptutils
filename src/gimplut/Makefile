# Makefile for gimplut
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.2 2012/01/22 19:41:59 jjg Exp jjg $

NAME     = gimplut
OBJ      = $(NAME).o main.o options.o
RUBBISH += $(OBJ) $(NAME)

LDLIBS += 
CFLAGS += 

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
main.o : options.h

# for testing 

GRAD     = Sunrise
RUBBISH += $(GRAD).lut

$(GRAD).lut : $(NAME)
	export GIMP_GRADIENTS="$(TESTD)/ggr" ; \
	./$(NAME) -o $(GRAD).lut -v $(GRAD)
check test : $(GRAD).lut 

include ../Common.mk
include ../Progs.mk



