# Makefile for svgx
# 
# J.J. Green 2005, 2008
# $Id: Makefile,v 1.21 2008/08/08 22:29:51 jjg Exp jjg $

OBJ      = svgx.o main.o options.o
CFLAGS  += 
LDLIBS  += 
RUBBISH += svgx $(OBJ) test/* test.png

default : all

SVGX   = ./svgx
SVGCPT = ./svgx -t cpt
SVGGGR = ./svgx -t ggr
SVGPSP = ./svgx -t psp
SVGPOV = ./svgx -t pov

CPTARG = -b10 -f1/2/3 -n0/12/0 

check test : svgx
	for file in `ls $(TESTD)/svg/*.svg` ; do \
	  echo $$file ; \
	  $(SVGX) -l $$file > /dev/null || exit 1 ; \
	  for name in `$(SVGX) -l $$file` ; do \
	    echo "  $$name" ; \
	    $(SVGCPT) -s $$name $(CPTARG) -o test/$$name.cpt $$file || exit 1 ; \
	    $(SVGGGR) -s $$name -o test/$$name.ggr $$file  || exit 1 ; \
	    $(SVGPOV) -s $$name -o test/$$name.inc $$file  || exit 1 ; \
	    $(SVGPSP) -s $$name -o test/$$name.grd $$file  || exit 1 ; \
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
	$(INSTALL) -m755 svgpsp.sh $(bindir)/svgpsp
	$(INSTALL) -m644 svgx.1 $(mandir)/man1/svgx.1 
	ln -f -s svgx.1 $(mandir)/man1/svgcpt.1
	ln -f -s svgx.1 $(mandir)/man1/svggimp.1
	ln -f -s svgx.1 $(mandir)/man1/svgpov.1
	ln -f -s svgx.1 $(mandir)/man1/svgpsp.1

else

all :
install :

endif

