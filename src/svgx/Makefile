# Makefile for svgx
# 
# J.J. Green 2005, 2008

NAME = svgx

OBJ      = svgx.o svgxdump.o png.o pngwrite.o main.o options.o utf8x.o
CFLAGS  += 
LDLIBS  += -lgrd3 -lcss3 -lgpt -lpov -lsvg -lsao -lggr -lcpt -lfill -lutf8 -lbt
RUBBISH += svgx $(OBJ) test.png

default : all

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
check test : accept

povray-test.png : povray-test.pov subtle.inc
	povray  +H480 +W640 +L. +A0.1 +Q11 +Ipovray-test.pov

subtle.inc :
	./svgx -t pov -o subtle.inc -v $(TESTD)/svg/subtle.svg

RUBBISH += povray-test.png subtle.inc

else

all :
install :
check test : 

endif

