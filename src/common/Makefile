# Makefile for libgimpcpt
# (c) J.J.Green 2001
# $Id: Makefile,v 1.2 2004/03/04 01:21:29 jjg Exp jjg $

OBJ      = bridge.o colour.o cpt.o cptparse.o cptscan.o cptio.o files.o fill.o gradient.o
RUBBISH += $(OBJ) libgimpcpt.a

default : all

all : libgimpcpt.a

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

libgimpcpt.a : $(OBJ)
	ar r libgimpcpt.a $(OBJ)

include ../Common.mk



