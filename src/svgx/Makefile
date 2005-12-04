# Makefile for svgx
# 
# J.J. Green 2005
# $Id: Makefile,v 1.16 2005/09/21 20:25:04 jjg Exp jjg $

OBJ      = svgx.o main.o options.o
CFLAGS  += 
LDLIBS  += 
RUBBISH += svgx $(OBJ) test/*.ggr test/*.cpt test/*.inc test.png

default : all

SVGX   = ./svgx
SVGCPT = ./svgx -t cpt
SVGGGR = ./svgx -t ggr
SVGPOV = ./svgx -t pov

check test : svgx
	for file in `ls test/*.svg` ; do \
	  echo $$file ; \
	  for name in `$(SVGX) -l $$file` ; do \
	    echo "  $$name" ; \
	    $(SVGCPT) -N $$name -b10 -f1/2/3 -n0/12/0 -o test/$$name.cpt $$file ; \
	    $(SVGGGR) -N $$name -o test/$$name.ggr $$file ; \
	    $(SVGPOV) -N $$name -o test/$$name.inc $$file ; \
	  done ;\
	done

test.png : test.pov
	povray  +L. +A0.1 +Q11 +Itest.pov

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
	$(INSTALL) -m755 svgpov.sh $(bindir)/svgpov
	$(INSTALL) -m644 svgx.1 $(mandir)/man1/svgx.1 
	ln -f -s svgx.1 $(mandir)/man1/svgcpt.1
	ln -f -s svgx.1 $(mandir)/man1/svggimp.1
	ln -f -s svgx.1 $(mandir)/man1/svgpov.1

else

all :
install :

endif

