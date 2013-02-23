# Makefile for xycpt
#
# (c) J.J.Green 2001,2004

OBJ      =  xycpt.o main.o options.o
RUBBISH += $(OBJ) xycpt

LDLIBS += -lcpt -lfill

default : all

all : xycpt xycpt.1

install :
	$(INSTALL_DATA) xycpt.1 $(DESTDIR)$(mandir)/man1/xycpt.1
	$(INSTALL_PROGRAM) xycpt $(DESTDIR)$(bindir)/xycpt

clean :
	$(RM) $(RUBBISH)
	for dir in i u ; do \
	  for n in 1 2 3 4 ; do \
	    $(RM) test/$$dir/$$n.cpt ; \
	  done ; \
	done

veryclean : 

.PHONEY    : all install clean veryclean

xycpt : $(OBJ)

# for testing 

test check : xycpt
	for n in 1 2 3 4 ; do \
	  ./xycpt -v -o    test/i/$$n.cpt test/i/$$n.dat || exit 1 ; \
	  ./xycpt -u -v -o test/u/$$n.cpt test/u/$$n.dat || exit 1 ; \
	done

include ../Common.mk


