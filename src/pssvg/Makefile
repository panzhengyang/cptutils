# Makefile for pssvg
# J.J. Green 2014

NAME = pssvg

OBJ       = options.o main.o pssvg.o ucs2utf8.o
CFLAGS   += 
LDLIBS   += -lsvg -lfill -lgrdx -lgrd5 -lgstk 
RUBBISH  += pssvg $(OBJ)
FIXTURES  = $(TESTD)/grd5

default : all

clean :
	$(RM) $(RUBBISH)

pssvg : $(OBJ)
	$(CC) $(LDFLAGS) -o pssvg $(OBJ) $(LDLIBS) 

include ../Common.mk

# depends on having libxml2

ifneq (no, $(XML2C))

all : pssvg pssvg.1 
install : all
	$(INSTALL) -m755 pssvg $(DESTDIR)$(bindir)/pssvg
	$(INSTALL) -m644 pssvg.1 $(DESTDIR)$(mandir)/man1/pssvg.1 
test check : accept

else

all :
install :
test check :

endif

