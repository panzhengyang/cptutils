# Makefile for gimpsvg
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.11 2011/11/10 23:28:05 jjg Exp jjg $

NAME = gimpsvg

OBJ      = $(NAME).o main.o options.o
RUBBISH += $(OBJ) $(NAME)

LDLIBS += 
CFLAGS += 

default : all

all : $(NAME)

install :
	$(INSTALL_PROGRAM) $(NAME) $(bindir)/$(NAME)
	$(INSTALL_DATA) $(NAME).1 $(mandir)/man1/$(NAME).1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY : all install clean veryclean

$(NAME) : $(OBJ)
main.o : options.h

# for testing 

GRAD     = Sunrise
RUBBISH += $(GRAD).svg $(GRAD).png
TOL      = 0

$(GRAD).svg : $(NAME)
	./$(NAME) -o $(GRAD).svg -T $(TOL) -v $(GRAD)

$(GRAD).png : $(GRAD).svg
	svgpng -v -1 -o $(GRAD).png $(GRAD).svg

showpng : $(GRAD).png ; feh $(GRAD).png 

check test : showpng

include ../Common.mk
include ../Progs.mk



