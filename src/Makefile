# Makefile for gimpcpt
# (c) J.J.Green 2001
# $Id: Makefile,v 1.1 2004/02/09 01:02:37 jjg Exp jjg $

SRC      = common gimpcpt cptgimp
RUBBISH += *~
COBFIG  += Common.mk

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






