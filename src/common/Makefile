# Makefile for cptutils common libraries
# (c) J.J.Green 2001
# $Id: Makefile,v 1.6 2005/01/27 21:32:27 jjg Exp jjg $

OBJFILL = colour.o fill.o
OBJCPT  = bridge.o cpt.o cptparse.o cptscan.o cptio.o files.o
OBJGGR  = gradient.o
OBJPSP  = psp.o

CPTULIBS = libfill.a libcpt.a libggr.a libpsp.a

RUBBISH += $(OBJFILL) $(OBJCPT) $(OBJGGR) $(CPTULIBS) $(OBJPSP)

default : all

all : $(CPTULIBS)

install :

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY : all test doc

cptscan.c cptscan.h : cptscan.l 
	flex cptscan.l 

cptparse.c : cptparse.y 
	bison cptparse.y

.SECONDARY : cptscan.c cptscan.h cptparse.c  

libfill.a : $(OBJFILL)
	ar r libfill.a $(OBJFILL)

libcpt.a : $(OBJCPT)
	ar r libcpt.a $(OBJCPT)

libggr.a : $(OBJGGR)
	ar r libggr.a $(OBJGGR)

libpsp.a : $(OBJPSP)
	ar r libpsp.a $(OBJPSP)

include ../Common.mk



