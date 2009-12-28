# Makefile for gimpcpt
# (c) J.J.Green 2010
# $Id: Makefile,v 1.14 2008/04/13 20:55:29 jjg Exp jjg $

SRC      = common avlcpt cptcss cptgimp cptinfo cpttext cptsvg cpthsv \
	   gimpcpt gimplut gplcpt pspcpt svgx xycpt
RUBBISH += *~
CONFIG  += Common.mk

default : all

all :
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) all || exit 1 ; cd .. ; \
	done

install :
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) install || exit 1 ; cd .. ; \
	done

clean :
	$(RM) $(RUBBISH)
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) clean || exit 1 ; cd .. ; \
	done

veryclean : 
	$(RM) $(CONFIG)






