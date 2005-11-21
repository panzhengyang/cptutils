# Makefile for cptutils common libraries
# (c) J.J.Green 2001
# $Id: Makefile,v 1.16 2005/11/21 22:47:52 jjg Exp jjg $

OBJFILL = colour.o fill.o 
OBJCPT  = bridge.o cpt.o cptparse.o cptscan.o cptio.o files.o
OBJGGR  = gradient.o
OBJPSP  = psp.o
OBJSVG  = svg.o svglist.o svgread.o svgwrite.o stdcol.o 
OBJPOV  = pov.o povwrite.o
OBJID   = ident.o identtab.o
OBJODB  = odb_bridge.o odb.o odbparse.o odbscan.o

CFLAGS +=
LDLIBS +=

CPTULIBS = libfill.a libcpt.a libggr.a libpsp.a libsvg.a libpov.a libident.a libodb.a

RUBBISH += $(OBJFILL) $(OBJCPT) $(OBJGGR) $(CPTULIBS) $(OBJPSP) $(OBJSVG) \
	   $(OBJPOV) $(OBJID) $(OBJODB)

default : all

all : $(CPTULIBS)

install :

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY : all test doc

cptscan.c cptscan.h : cptscan.l 
	flex cptscan.l 

odbscan.c odbscan.h : odbscan.l 
	flex odbscan.l 

cptparse.c : cptparse.y 
	bison cptparse.y

odbparse.c : odbparse.y 
	bison odbparse.y

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

libident.a : $(OBJID)
	ar r libident.a $(OBJID)

libodb.a : $(OBJODB)
	ar r libodb.a $(OBJODB)

stdcol.c : stdcol.gperf
	$(GPERF) -e "," -t -L ANSI-C --ignore-case stdcol.gperf > stdcol.c

RUBBISH += svgio arse.svg

svgio : svgio.o svg.o colour.o


include ../Common.mk



