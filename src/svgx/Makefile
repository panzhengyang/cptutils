# Makefile for svgcpt
# $Id: Makefile,v 1.2 2004/09/06 23:18:52 jjg Exp jjg $

OBJ      = svgcpt.o main.o options.o
GRAD     = gradient-pastel-blue.svg
CFLAGS  += `$(XML2C) --cflags`
LDFLAGS += `$(XML2C) --libs`
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
