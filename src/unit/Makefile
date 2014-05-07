# Makefile for cunit test-suite

LDLIBS  += -lcpt -lfill
LDFLAGS += -L../lib/
CFLAGS  += -I../common/

OBJ = options.o main.o tests.o \
  fixture.o fixture_cpt.o \
  tests_colour.o tests_colour_helper.o \
  tests_fill.o tests_fill_helper.o \
  tests_cpt.o tests_cpt_helper.o \
  tests_cptread.o  \
  tests_cptwrite.o \
  tests_cptname.o \
  tests_gmtcol.o

RUBBISH += *~ $(OBJ) unit-test

default : all

include ../Common.mk

all : unit-test

unit : unit-test
	./unit-test -v

ifdef WITH_UNIT

unit-test : $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) $(LDLIBS) -o unit-test

else

unit-test : 
	echo "no CUnit support, use --enable-tests"
	exit 1

endif

clean spotless veryclean : 
	$(RM) $(RUBBISH)

