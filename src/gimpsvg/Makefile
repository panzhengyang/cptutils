# Makefile for gimpcpt
# (c) J.J.Green 2001,2004
# $Id: Makefile,v 1.3 2004/02/10 00:18:21 jjg Exp jjg $

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

# for testing 

GRAD     = Sunrise
RUBBISH += $(GRAD).cpt $(GRAD).ps $(GRAD).png

$(GRAD).cpt : gimpcpt
	./gimpcpt -o $(GRAD).cpt -v $(GRAD)

$(GRAD).ps : $(GRAD).cpt defaults
	psscale +defaults -D1/6/10/0.5 -Bf5a10 -C$< -E > $@

$(GRAD).png : $(GRAD).ps
	convert -trim -border 5x5 -bordercolor white $(GRAD).ps $(GRAD).png

showps  : $(GRAD).ps  ; gv  $(GRAD).ps 
showpng : $(GRAD).png ; feh $(GRAD).png 

check test : showpng

include ../Common.mk
include ../Progs.mk



