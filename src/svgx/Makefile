# Makefile for svgcpt
# $Id: Makefile,v 1.1 2004/09/06 21:45:04 jjg Exp jjg $

RUBBISH  = *~ svgcpt
CFLAGS  += -Wall `xml2-config --cflags`
LDFLAGS += `xml2-config --libs`
GRAD     = gradient-pastel-blue.svg

default : all

all : svgcpt

check test : svgcpt
	./svgcpt $(GRAD)

clean :
	$(RM) $(RUBBISH)
