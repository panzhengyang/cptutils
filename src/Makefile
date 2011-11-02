# Makefile for gimpcpt
# (c) J.J.Green 2010
# $Id: Makefile,v 1.18 2010/09/14 18:39:12 jjg Exp jjg $

SRC      = common avlcpt cptclip cptcont cptcss cptgimp cptinfo \
	   cpttext cptsvg cpthsv gimpcpt gimplut gplcpt pspsvg \
	   svgx xycpt
RUBBISH += *~
CONFIG  += Common.mk

default : all

all :
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) all || exit 1 ; cd .. ; \
	done

install : install-dirs
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) install || exit 1 ; cd .. ; \
	done

install-dirs :
	for dir in $(bindir) $(mandir) $(mandir)/man1 ; do \
	  $(INSTALL_DIR) $$dir ; \
	done

clean :
	$(RM) $(RUBBISH)
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) clean || exit 1 ; cd .. ; \
	done

veryclean : 
	$(RM) $(CONFIG)

include Common.mk




