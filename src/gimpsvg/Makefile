# Makefile for gimpcpt
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.5 2004/02/24 02:13:51 jjg Exp jjg $

OBJ      = gimpcpt.o main.o dp-simplify.o findgrad.o options.o
RUBBISH += $(OBJ) gimpcpt

default : all

all : gimpcpt

install :
	$(INSTALL_PROGRAM) gimpcpt $(bindir)/gimpcpt
	$(INSTALL_DATA) gimpcpt.1 $(mandir)/man1/gimpcpt.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean
.SECONDARY : options.c options.h

gimpcpt : $(OBJ)
main.o : options.h

# for testing 

GRAD     = Blinds
RUBBISH += $(GRAD).cpt $(GRAD).ps $(GRAD).png
TOL      = 1.0

$(GRAD).cpt : gimpcpt
	./gimpcpt -o $(GRAD).cpt -T $(TOL) -v $(GRAD)

$(GRAD).ps : $(GRAD).cpt defaults
	psscale +defaults -D6/1/10/0.7h -Ba0.2 -C$< -E > $@

$(GRAD).png : $(GRAD).ps
	convert -trim -border 5x5 -bordercolor white $(GRAD).ps $(GRAD).png

showps  : $(GRAD).ps  ; gv  $(GRAD).ps 
showpng : $(GRAD).png ; feh $(GRAD).png 

check test : showpng

include ../Common.mk
include ../Progs.mk



