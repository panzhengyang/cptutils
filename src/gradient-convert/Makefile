NAME = gradient-convert

RUBBISH = $(NAME)

all : $(NAME) $(NAME).1

$(NAME) : $(NAME).py
	sed s/VERSION/$(VERSION)/g $(NAME).py > $(NAME)
	chmod 755 $(NAME)  

install :
	$(INSTALL_DATA) $(NAME).1 $(DESTDIR)$(mandir)/man1/$(NAME).1
	$(INSTALL_PROGRAM) $(NAME) $(DESTDIR)$(bindir)/$(NAME)

clean :
	$(RM) $(RUBBISH)

# this script requires that the other programs have 
# already been installed, so the "standard" test target
# does no conversion, it just prints the version

test check :
	./$(NAME) --version

include ../Common.mk
