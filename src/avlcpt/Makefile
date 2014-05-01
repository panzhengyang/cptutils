# Makefile for avlcpt
#
# (c) J.J.Green 2001,2004

NAME     = avlcpt
OBJ      = avlcpt.o main.o options.o avlgrad.o
RUBBISH += $(OBJ) avlcpt

LDLIBS  += -lcpt -lfill -lodb -lident

default : all

all : avlcpt avlcpt.1

install :
	$(INSTALL_DATA) avlcpt.1 $(DESTDIR)$(mandir)/man1/avlcpt.1
	$(INSTALL_PROGRAM) avlcpt $(DESTDIR)$(bindir)/avlcpt

clean : 
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

avlcpt : $(OBJ)

.SECONDARY : odbscan.c odbscan.h odbparse.c

odbscan.c odbscan.h : odbscan.l 
	flex odbscan.l 

odbparse.c : odbparse.y 
	bison odbparse.y

include ../Common.mk

test check : accept


