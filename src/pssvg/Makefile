# Makefile for pssvg
# 
# J.J. Green 2014

OBJ      = options.o grd5_type.o
CFLAGS  += 
LDLIBS  += -lsvg
RUBBISH += pssvg $(OBJ) test/*

default : all

check test : pspsvg

clean :
	$(RM) $(RUBBISH)

pssvg : $(OBJ)

include ../Common.mk

# depends on having libxml2

ifneq (no,$(XML2C))

all : pssvg pssvg.1 
install : all
	$(INSTALL) -m755 pssvg $(DESTDIR)$(bindir)/pssvg
	$(INSTALL) -m644 pssvg.1 $(DESTDIR)$(mandir)/man1/pssvg.1 

else

all :
install :

endif

