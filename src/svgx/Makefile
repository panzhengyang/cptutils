# Makefile for svgcpt
# $Id: Makefile,v 1.3 2004/09/07 15:52:38 jjg Exp jjg $

OBJ      = svgcpt.o main.o options.o
GRAD     = gradient-pastel-blue.svg
CFLAGS  += `$(XML2C) --cflags`
LDLIBS  += `$(XML2C) --libs`
RUBBISH += svgcpt $(OBJ)

default : all

check test : svgcpt
	./svgcpt $(GRAD)

clean :
	$(RM) $(RUBBISH)

svgcpt : $(OBJ)

include ../Common.mk
include ../Progs.mk

ifneq (no,$(XML2C))
all : svgcpt
else
all :
	echo "No libxml2, skipping svgcpt"
endif
