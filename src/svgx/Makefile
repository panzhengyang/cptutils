# Makefile for svgx
# 
# J.J. Green 2005
# $Id: Makefile,v 1.7 2005/06/23 23:14:06 jjg Exp jjg $

OBJ      = svgx.o main.o options.o
GRAD     = test/gradient-pastel-blue.svg
IMAGE    = test/eyes.svg
CFLAGS  += 
LDLIBS  += 
RUBBISH += svgx test.cpt linearGradient*.cpt $(OBJ)

default : all

SVGCPT = ./svgx -t cpt
SVGGGR = ./svgx -t ggr

check test : svgx
	$(SVGCPT) -v -l $(IMAGE)
	$(SVGCPT) -v -n linearGradient1612 $(IMAGE)
	$(SVGCPT) -v -a $(IMAGE)
	$(SVGCPT) -v -l $(GRAD)
	$(SVGCPT) -v -n pastel-blue -o test.cpt $(GRAD)

clean :
	$(RM) $(RUBBISH)

svgx : $(OBJ)

include ../Common.mk
include ../Progs.mk

# depends on having libxml2

ifneq (no,$(XML2C))

all : svgx
install : all
	$(INSTALL) -m755 svgx $(bindir)/svgx
	$(INSTALL) -m755 svgcpt.sh $(bindir)/svgcpt
	$(INSTALL) -m755 svggimp.sh $(bindir)/svggimp
	$(INSTALL) -m644 svgx.1 $(mandir)/man1/svgx.1 

else

all :
install :

endif

