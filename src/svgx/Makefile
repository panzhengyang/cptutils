# Makefile for svgcpt
# $Id: Makefile,v 1.4 2004/09/07 23:04:48 jjg Exp jjg $

OBJ      = svgcpt.o main.o options.o
GRAD     = gradient-pastel-blue.svg
CFLAGS  += 
LDLIBS  += 
RUBBISH += svgcpt $(OBJ)

default : all

check test : svgcpt
	./svgcpt -v -l $(GRAD)
	./svgcpt -v -n1 -o test.cpt $(GRAD)

clean :
	$(RM) $(RUBBISH)

svgcpt : $(OBJ)

include ../Common.mk
include ../Progs.mk

# depends on having libxml2

ifneq (no,$(XML2C))

all : svgcpt
install :

else

all :
install :

endif

