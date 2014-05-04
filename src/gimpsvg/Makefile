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

include ../Common.mk

check test : accept


