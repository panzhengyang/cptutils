# Makefile for cptutils common libraries
# (c) J.J.Green 2001
# $Id: Makefile,v 1.4 2004/04/01 21:32:20 jjg Exp jjg $

OBJFILL = colour.o fill.o
OBJCPT  = bridge.o cpt.o cptparse.o cptscan.o cptio.o files.o
OBJGGR  = gradient.o

CPTULIBS = libfill.a libcpt.a libggr.a

RUBBISH += $(OBJFILL) $(OBJCPT) $(OBJGGR) $(CPTULIBS)

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

include ../Common.mk



