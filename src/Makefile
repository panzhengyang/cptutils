# Makefile for gimpcpt
# (c) J.J.Green 2010
# $Id: Makefile,v 1.23 2012/03/09 21:52:45 jjg Exp jjg $

SRC      = common avlcpt cptclip cptcont cptcss cptinfo \
	   cpttext cptsvg cpthsv gimpsvg gimplut gplcpt \
	   pspsvg svgx xycpt
RUBBISH += *~
CONFIG  += Common.mk

ifneq (no,$(PYTHON))

SRC += gradient-convert

endif

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
	  $(INSTALL_DIR) $(DESTDIR)$$dir ; \
	done

clean :
	$(RM) $(RUBBISH)
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) clean || exit 1 ; cd .. ; \
	done

test check :
	for dir in $(SRC) ; do \
	  cd $$dir ; $(MAKE) test || exit 1 ; cd .. ; \
	done

veryclean : 
	$(RM) $(CONFIG)

include Common.mk




