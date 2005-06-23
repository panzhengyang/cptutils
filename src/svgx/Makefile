# Makefile for svgcpt
# 
# J.J. Green 2005
# $Id: Makefile,v 1.6 2005/06/13 22:36:00 jjg Exp jjg $

OBJ      = svgcpt.o main.o options.o
GRAD     = test/gradient-pastel-blue.svg
IMAGE    = test/eyes.svg
CFLAGS  += 
LDLIBS  += 
RUBBISH += svgcpt test.cpt $(OBJ)

default : all

check test : svgcpt
	./svgcpt -v -l $(IMAGE)
	./svgcpt -v -n linearGradient1612 $(IMAGE)
	./svgcpt -v -l $(GRAD)
	./svgcpt -v -n pastel-blue -o test.cpt $(GRAD)

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

