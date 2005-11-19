# Makefile for gimpcpt
# (c) J.J.Green 2001
# $Id: Makefile,v 1.11 2005/07/19 22:26:45 jjg Exp jjg $

SRC      = common avlcpt cptcss cptgimp cptinfo cpttext cptsvg \
	   gimpcpt pspcpt svgx xycpt
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






