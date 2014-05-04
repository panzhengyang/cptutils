# Makefile for gimpcpt
# (c) J.J.Green 2010

SRC      = common avlcpt cptcat cptclip cptcont cptcss \
	   cptinfo cpttext cptsvg cpthsv gimpsvg gimplut \
	   gplcpt pspsvg pssvg svgx xycpt
RUBBISH += *~
CONFIG  += Common.mk

ifneq (no,$(PYTHON))

SRC += gradient-convert

endif

default : all

all :
	for dir in $(SRC) ; do $(MAKE) -C $$dir ; done

install : install-dirs
	for dir in $(SRC) ; do $(MAKE) -C $$dir install ; done

install-dirs :
	for dir in $(bindir) $(mandir) $(mandir)/man1 ; do \
	  $(INSTALL_DIR) $(DESTDIR)$$dir ; \
	done

clean :
	$(RM) $(RUBBISH)
	for dir in $(SRC) unit ; do $(MAKE) -C $$dir clean ; done

test check :
	for dir in $(SRC) ; do $(MAKE) -C $$dir test || exit 1 ; done

unit :
	$(MAKE) -C unit unit

veryclean : 
	$(RM) $(CONFIG)

.PHONY : all install install-dirs clean test check unit

include Common.mk




