# Makefile for cunit test-suite

LDLIBS  += 
LDFLAGS += -L../lib/
CFLAGS  += -I../common/

OBJ = options.o main.o tests.o template.o

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

