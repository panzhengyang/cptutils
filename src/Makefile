# Makefile for gimpcpt
# (c) J.J.Green 2001
# $Id: Makefile,v 1.5 2004/04/12 20:10:20 jjg Exp jjg $

SRC      = common gimpcpt cptgimp cptinfo cpttext xycpt
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






