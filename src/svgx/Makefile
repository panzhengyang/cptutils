# Makefile for svgx
# 
# J.J. Green 2005, 2008

OBJ      = svgx.o svgxdump.o png.o pngwrite.o main.o options.o utf8x.o
CFLAGS  += 
LDLIBS  += -lgrd3 -lcss3 -lgpt -lpov -lsvg -lsao -lggr -lcpt -lfill -lutf8
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
SVGSVG  = ./svgx -t svg

TSPARG = -T255/0/0
CPTARG = -b10 -f1/2/3 -n0/12/0 $(TSPARG)

check test : svgx test-named test-list test-mad-ids test-all

test-all :
	for file in Gradients_01 ; do \
	  path="$(TESTD)/svg/$$file.svg" ; \
	  echo $$path ; \
	  $(SVGCPT) -a -o test $$path || exit 1 ; \
	done

test-list :
	for file in eyes ; do \
	  path="$(TESTD)/svg/$$file.svg" ; \
	  echo $$path ; \
	  $(SVGCPT) -l $$path > /dev/null || exit 1 ; \
	done

test-named :
	for file in eyes ; do \
	  path="$(TESTD)/svg/$$file.svg" ; \
	  echo $$path ; \
	  for name in `$(SVGX) -l $$path` ; do \
	    echo "  $$name" ; \
	    $(SVGCPT) -s $$name $(CPTARG) -o test/$$name.cpt $$path \
		|| exit 1 ; \
	    $(SVGGGR) -s $$name -o test/$$name.ggr $$path  || exit 1 ; \
	    $(SVGPOV) -s $$name -o test/$$name.inc $$path  || exit 1 ; \
	    $(SVGGPT) -s $$name $(TSPARG) -o test/$$name.gpt $$path  \
		|| exit 1 ; \
	    $(SVGCSS3) -s $$name -o test/$$name.css3 $$path  || exit 1 ; \
	    $(SVGPSP) -s $$name -o test/$$name.grd $$path  || exit 1 ; \
	    $(SVGSAO) -s $$name $(TSPARG) -o test/$$name.sao $$path  \
		|| exit 1 ; \
	    $(SVGPNG) -s $$name -o test/$$name.png $$path  || exit 1 ; \
	    $(SVGSVG) -s $$name -p -o test/$$name.svg $$path  || exit 1 ; \
	  done ;\
	done

test-mad-ids :
	$(SVGSVG) -a -o test $(TESTD)/svg/mad-ids.svg

test.png : test.pov
	povray  +H480 +W640 +L. +A0.1 +Q11 +Itest.pov

clean :
	$(RM) $(RUBBISH)

svgx : $(OBJ)

include ../Common.mk

# depends on having libxml2

ifneq (no,$(XML2C))

all : svgx svgx.1 
install : all
	$(INSTALL) -m755 svgx $(DESTDIR)$(bindir)/svgx
	$(INSTALL) -m644 svgx.1 $(DESTDIR)$(mandir)/man1/svgx.1 
	for type in cpt css3 gimp gpt png pov psp sao svg ; do \
	  base="svg$$type" ; \
	  $(INSTALL) -m755 wrapper/$$base.sh $(DESTDIR)$(bindir)/$$base ; \
	  ln -f -s svgx.1 $(DESTDIR)$(mandir)/man1/$$base.1 ; \
	done

else

all :
install :

endif

