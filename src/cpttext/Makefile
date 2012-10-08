# Makefile for cpttext
# (c) J.J.Green 2001,2004,2012
# $Id: Makefile,v 1.5 2012/02/27 19:01:06 jjg Exp jjg $

OBJ      = main.o options.o cpttext.o
RUBBISH += $(OBJ) cpttext test.html

LDLIBS += 

default : all

all : cpttext cpttext.1

install :
	$(INSTALL_PROGRAM) cpttext $(DESTDIR)$(bindir)/cpttext
	$(INSTALL_DATA) cpttext.1 $(DESTDIR)$(mandir)/man1/cpttext.1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

cpttext : $(OBJ)

# for testing 

CPT = $(TESTD)/cpt/subtle.cpt

test check : test.html

test.html : cpttext 
	echo "<html>" >  test.html
	echo "<body>" >> test.html
	./cpttext -t "Lorem ipsum dolor sit amet" $(CPT) >> test.html
	echo "</body>" >> test.html
	echo "</html>" >> test.html

include ../Progs.mk
include ../Common.mk