# Makefile for cptclip
# (c) J.J.Green 2013

NAME = cptcat

OBJ      = $(NAME).o main.o options.o
RUBBISH += $(OBJ) $(NAME)

CFLAGS  += 
LDLIBS  += -lcpt -lfill

default : all

all : $(NAME) $(NAME).1

install :
	$(INSTALL_PROGRAM) $(NAME) $(DESTDIR)$(bindir)/$(NAME)
	$(INSTALL_DATA) $(NAME).1 $(DESTDIR)$(mandir)/man1/$(NAME).1

clean :
	$(RM) $(RUBBISH)

veryclean : 

.PHONEY    : all install clean veryclean

$(NAME) : $(OBJ)

include ../Common.mk

CPTBATH = $(TESTD)/cpt/tpsfhm.cpt
CPTTOPO = $(TESTD)/cpt/pakistan.cpt
CPTA    = tpsfhm-pakistan.cpt
CPTB    = pakistan-tpsfhm.cpt

test : $(CPTA) $(CPTB)

$(CPTA) : $(NAME)
	./$(NAME) -o $(CPTA) -v $(CPTBATH) $(CPTTOPO)

$(CPTB) : $(NAME)
	./$(NAME) -o $(CPTB) -v $(CPTTOPO) $(CPTBATH)

RUBBISH += $(CPTA) $(CPTB)



