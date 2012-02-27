# Makefile for svgx
# 
# J.J. Green 2005, 2008
# $Id: Makefile,v 1.33 2012/02/27 16:15:30 jjg Exp jjg $

OBJ      = svgx.o png.o pngwrite.o main.o options.o utf8x.o
CFLAGS  += 
LDLIBS  += 
RUBBISH += svgx $(OBJ) test/* test.png

default : all

SVGX    = ./svgx
SVGCPT  = ./svgx -t cpt
SVGCSS3 = ./svgx -t css3
SVGGGR  = ./svgx -t ggr
SVGGPT  = ./svgx -t gpt
SVGPNG  = ./svgx -t png
SVGPOV  = ./svgx -t pov
SVGPSP  = ./svgx -t psp
SVGSAO  = ./svgx -t sao

TSPARG = -T255/0/0
CPTARG = -b10 -f1/2/3 -n0/12/0 $(TSPARG)

check test : svgx
	for file in `ls $(TESTD)/svg/*.svg` ; do \
	  echo $$file ; \
	  $(SVGX) -l $$file > /dev/null || exit 1 ; \
	  for name in `$(SVGX) -l $$file` ; do \
	    echo "  $$name" ; \
	    $(SVGCPT) -s $$name $(CPTARG) -o test/$$name.cpt $$file \
		|| exit 1 ; \
	    $(SVGGGR) -s $$name -o test/$$name.ggr $$file  || exit 1 ; \
	    $(SVGPOV) -s $$name -o test/$$name.inc $$file  || exit 1 ; \
	    $(SVGGPT) -s $$name $(TSPARG) -o test/$$name.gpt $$file  \
		|| exit 1 ; \
	    $(SVGCSS3) -s $$name -o test/$$name.css3 $$file  || exit 1 ; \
	    $(SVGPSP) -s $$name -o test/$$name.grd $$file  || exit 1 ; \
	    $(SVGSAO) -s $$name $(TSPARG) -o test/$$name.sao $$file  \
		|| exit 1 ; \
	    $(SVGPNG) -s $$name -o test/$$name.png $$file  || exit 1 ; \
	  done ;\
	done

test.png : test.pov
	povray  +H480 +W640 +L. +A0.1 +Q11 +Itest.pov

clean :
	$(RM) $(RUBBISH)

svgx : $(OBJ)

include ../Common.mk
include ../Progs.mk

# depends on having libxml2

ifneq (no,$(XML2C))

all : svgx svgx.1 
install : all
	$(INSTALL) -m755 svgx $(DESTDIR)$(bindir)/svgx
	$(INSTALL) -m644 svgx.1 $(DESTDIR)$(mandir)/man1/svgx.1 
	for type in cpt css3 gimp gpt png pov psp sao ; do \
	  base="svg$$type" ; \
	  $(INSTALL) -m755 wrapper/$$base.sh $(DESTDIR)$(bindir)/$$base ; \
	  ln -f -s svgx.1 $(DESTDIR)$(mandir)/man1/$$base.1 ; \
	done

else

all :
install :

endif

