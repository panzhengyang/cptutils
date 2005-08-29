# Makefile for svgx
# 
# J.J. Green 2005
# $Id: Makefile,v 1.12 2005/08/29 21:23:27 jjg Exp jjg $

OBJ      = svgx.o main.o options.o
CFLAGS  += 
LDLIBS  += 
RUBBISH += svgx $(OBJ) test/*.ggr test/*.cpt test/*.inc test.png

default : all

SVGCPT = ./svgx -t cpt
SVGGGR = ./svgx -t ggr
SVGPOV = ./svgx -t pov

check test : svgx
	for file in `ls test/*.svg` ; do \
	  echo $$file ; \
	  for name in `$(SVGCPT) -l $$file` ; do \
	    echo "  $$name" ; \
	    $(SVGCPT) -n $$name -o test/$$name.cpt $$file ; \
	    $(SVGGGR) -n $$name -o test/$$name.ggr $$file ; \
	    $(SVGPOV) -n $$name -o test/$$name.inc $$file ; \
	  done ;\
	done

test.png : test.pov
	povray  +L, +Itest.pov

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

