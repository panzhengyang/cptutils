# Makefile for gimpcpt
# (c) J.J.Green 2001
# $Id: Makefile,v 1.9 2005/05/30 23:18:08 jjg Exp jjg $

SRC      = common gimpcpt cptgimp cptinfo cpttext cptsvg \
	   svgx pspcpt xycpt
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






