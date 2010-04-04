# Makefile for gimpcpt
# (c) J.J.Green 2010
# $Id: Makefile,v 1.15 2009/12/28 21:53:24 jjg Exp jjg $

SRC      = common avlcpt cptcont cptcss cptgimp cptinfo cpttext cptsvg \
	   cpthsv gimpcpt gimplut gplcpt pspcpt svgx xycpt
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






