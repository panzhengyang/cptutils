# Makefile for libgimpcpt
# (c) J.J.Green 2001
# $Id: Makefile.in,v 1.2 2002/07/23 00:09:53 jjg Exp $

OBJ = colour.o cpt.o files.o gradient.o

RUBBISH += $(OBJ) libgimpcpt.a

default : all

all : libgimpcpt.a

install :

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY : all test doc

libgimpcpt.a : $(OBJ)
	ar r libgimpcpt.a $(OBJ)

include ../Common.mk



