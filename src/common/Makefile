# Makefile for cptutils common libraries
# (c) J.J.Green 2001

OBJFILL = colour.o fill.o 
OBJCPT  = cptbridge.o cpt.o cptscan.o cptparse.o cptio.o
OBJGGR  = ggr.o findggr.o  files.o
OBJPSP  = pspread.o pspwrite.o psp.o htons.o
OBJSVG  = svg.o svglist.o svgread.o svgwrite.o stdcol.o svgpreview.o
OBJPOV  = pov.o povwrite.o
OBJGPT  = gpt.o gptwrite.o
OBJCSS3 = css3.o css3write.o
OBJID   = ident.o identtab.o
OBJODB  = odbbridge.o odb.o odbscan.o odbparse.o 
OBJSAO  = sao.o saowrite.o

CFLAGS += 
LDLIBS +=

CPTULIBS = libfill.a libcpt.a libggr.a libpsp.a libsvg.a libpov.a \
	libident.a libodb.a libgpt.a libcss3.a libsao.a

RUBBISH += $(OBJFILL) $(OBJCPT) $(OBJGGR) $(CPTULIBS) $(OBJPSP) $(OBJSVG) \
	   $(OBJPOV) $(OBJID) $(OBJODB) $(OBJGPT) $(OBJCSS3) $(OBJSAO) 

default : all

all : $(CPTULIBS)

install :

test check :

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY : all test doc

cptscan.c cptscan.h : cptscan.l 
	flex cptscan.l 

odbscan.c odbscan.h : odbscan.l 
	flex odbscan.l 

cptparse.c cptparse,h : cptparse.y 
	bison cptparse.y

odbparse.c odbparse.h : odbparse.y 
	bison odbparse.y

odbparse.o : odbparse.c odbscan.h
odbscan.o  : odbscan.c odbparse.h

cptparse.o : cptparse.c cptscan.h
cptscan.o  : cptscan.c cptparse.h

.SECONDARY : cptscan.c cptscan.h cptparse.c stdcol.c

libfill.a : $(OBJFILL)
	ar r libfill.a $(OBJFILL)

libcpt.a : $(OBJCPT)
	ar r libcpt.a $(OBJCPT)

libggr.a : $(OBJGGR)
	ar r libggr.a $(OBJGGR)

libpsp.a : $(OBJPSP)
	ar r libpsp.a $(OBJPSP)

libsvg.a : $(OBJSVG)
	ar r libsvg.a $(OBJSVG)

libpov.a : $(OBJPOV)
	ar r libpov.a $(OBJPOV)

libgpt.a : $(OBJGPT)
	ar r libgpt.a $(OBJGPT)

libcss3.a : $(OBJCSS3)
	ar r libcss3.a $(OBJCSS3)

libident.a : $(OBJID)
	ar r libident.a $(OBJID)

libodb.a : $(OBJODB)
	ar r libodb.a $(OBJODB)

libsao.a : $(OBJSAO)
	ar r libsao.a $(OBJSAO)

stdcol.c : stdcol.gperf
	$(GPERF) -e "," -t -L ANSI-C --ignore-case stdcol.gperf > stdcol.c

include ../Common.mk



