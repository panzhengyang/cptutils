# Makefile for cptutils common libraries
# (c) J.J.Green 2001

OBJFILL = colour.o fill.o 
OBJCPT  = cpt.o cptread.o cptwrite.o gmtcol.o cptname.o
OBJGGR  = ggr.o
OBJGRD3 = grd3read.o grd3write.o grd3.o
OBJGRD5 = grd5type.o grd5read.o grd5string.o grd5.o
OBJGRDX = grdxsvg.o
OBJGSTK = gstack.o
OBJSVG  = svg.o svglist.o svgread.o svgwrite.o stdcol.o svgpreview.o
OBJPOV  = pov.o povwrite.o
OBJGPT  = gpt.o gptwrite.o
OBJCSS3 = css3.o css3write.o
OBJID   = ident.o identtab.o
OBJODB  = odbbridge.o odb.o odbscan.o odbparse.o 
OBJSAO  = sao.o saowrite.o
OBJUTF8 = utf8.o
OBJBT   = btrace.o

CFLAGS += 
LDLIBS +=

CPTULIBS = libfill.a libcpt.a libggr.a libgrd3.a libgrd5.a libgrdx.a \
	libgstk.a libsvg.a libpov.a libident.a libodb.a libgpt.a \
	libcss3.a libsao.a libutf8.a libbt.a

RUBBISH +=  $(CPTULIBS) $(OBJFILL) $(OBJCPT) $(OBJGGR) \
	   $(OBJGRD3) $(OBJGRD5) $(OBJGRDX) \
	   $(OBJSVG) $(OBJPOV) $(OBJID) $(OBJODB) $(OBJGPT) $(OBJCSS3) \
	   $(OBJSAO) $(OBJGSTK) $(OBJUTF8) $(OBJBT)

default : all

all : $(CPTULIBS)

install :

test check :

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY : all test doc

odbscan.c odbscan.h : odbscan.l 
	flex odbscan.l 

odbparse.c odbparse.h : odbparse.y 
	bison odbparse.y

odbparse.o : odbparse.c odbscan.h
odbscan.o  : odbscan.c odbparse.h

libfill.a : $(OBJFILL)
	ar r libfill.a $(OBJFILL)

libcpt.a : $(OBJCPT)
	ar r libcpt.a $(OBJCPT)

libggr.a : $(OBJGGR)
	ar r libggr.a $(OBJGGR)

libgrdx.a : $(OBJGRDX)
	ar r libgrdx.a $(OBJGRDX)

libgrd3.a : $(OBJGRD3)
	ar r libgrd3.a $(OBJGRD3)

libgrd5.a : $(OBJGRD5)
	ar r libgrd5.a $(OBJGRD5)

libgstk.a : $(OBJGSTK)
	ar r libgstk.a $(OBJGSTK)

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

libutf8.a : $(OBJUTF8)
	ar r libutf8.a $(OBJUTF8)

libbt.a : $(OBJBT)
	ar r libbt.a $(OBJBT)

include ../Common.mk



