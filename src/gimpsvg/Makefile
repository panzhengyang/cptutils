# Makefile for gimpsvg
# (c) J.J.Green 2001,2004

NAME = gimpsvg

OBJ      = $(NAME).o main.o options.o
RUBBISH += $(OBJ) $(NAME)

LDLIBS += -lsvg -lggr -lfill
CFLAGS += 

default : all

all : $(NAME) $(NAME).1

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

GRAD     = Sunrise
RUBBISH += $(GRAD).svg $(GRAD).png
GGRDIR   = $(TESTD)/ggr

$(GRAD).svg : $(NAME)
	export GIMP_GRADIENTS="$(GGRDIR)" ; \
	  ./$(NAME) -p -o $(GRAD).svg -v $(GRAD)

$(GRAD).png : $(GRAD).svg
	svgpng -v -o $(GRAD).png $(GRAD).svg

showpng : $(GRAD).png ; feh $(GRAD).png 

check test : $(GRAD).svg 

include ../Common.mk


