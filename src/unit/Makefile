# Makefile for cunit test-suite

LDLIBS  += -lcpt -lfill -lcss3 -lggr -lgpt -lgrd3 -lgrd5 \
           -lgstk -lgrdx -lident -lpov -lsao -lsvg

LDFLAGS += -L../lib/
CFLAGS  += -I../common/

OBJ = options.o main.o tests.o \
  fixture.o \
  tests_colour.o tests_colour_helper.o \
  tests_cpt.o tests_cpt_helper.o fixture_cpt.o \
  tests_cptread.o  \
  tests_cptwrite.o \
  tests_cptname.o \
  tests_css3write.o \
  tests_fill.o tests_fill_helper.o \
  tests_ggr.o fixture_ggr.o \
  tests_gmtcol.o \
  tests_gpt.o \
  tests_gptwrite.o \
  tests_grd3.o fixture_grd3.o \
  tests_grd3read.o \
  tests_grd3write.o \
  tests_grd5.o tests_grd5_helper.o fixture_grd5.o \
  tests_grd5read.o \
  tests_grd5string.o \
  tests_grd5type.o \
  tests_grdxsvg.o \
  tests_gstack.o \
  tests_ident.o \
  tests_identtab.o tests_identtab_helper.o \
  tests_pov.o \
  tests_povwrite.o \
  tests_sao.o tests_sao_helper.o \
  tests_saowrite.o \
  tests_stdcol.o \
  tests_svg.o tests_svg_helper.o \
  tests_svglist.o tests_svglist_helper.o \
  tests_svgpreview.o


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

