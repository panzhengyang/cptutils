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
	for dir in $(SRC) ; do \
	  $(MAKE) -C $$dir || exit 1 ; \
	done

install : install-dirs
	for dir in $(SRC) ; do \
	  $(MAKE) -C $$dir install || exit 1 ; \
	done

install-dirs :
	for dir in $(bindir) $(mandir) $(mandir)/man1 ; do \
	  $(INSTALL_DIR) $(DESTDIR)$$dir || exit 1 ; \
	done

clean :
	$(RM) $(RUBBISH)
	for dir in $(SRC) unit ; do \
	  $(MAKE) -C $$dir clean || exit 1 ; \
	done

test check :
	for dir in $(SRC) ; do \
	  $(MAKE) -C $$dir test || exit 1 ; \
	done

unit :
	$(MAKE) -C unit unit

veryclean : 
	$(RM) $(CONFIG)

.PHONY : all install install-dirs clean test check unit

include Common.mk

