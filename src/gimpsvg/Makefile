# Makefile for gimpsvg
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.14 2011/11/11 22:41:47 jjg Exp jjg $

NAME = gimpsvg

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

.PHONEY : all install clean veryclean

$(NAME) : $(OBJ)
main.o : options.h

# for testing 

GRAD     = Full_saturation_spectrum_CCW
RUBBISH += $(GRAD).svg $(GRAD).png
TOL      = 0

$(GRAD).svg : $(NAME)
	./$(NAME) -o $(GRAD).svg -v $(GRAD)

$(GRAD).png : $(GRAD).svg
	svgpng -v -1 -o $(GRAD).png $(GRAD).svg

showpng : $(GRAD).png ; feh $(GRAD).png 

check test : showpng

include ../Common.mk
include ../Progs.mk



