NAME = gradient-convert

RUBBISH = $(NAME)

all : $(NAME)

$(NAME) : $(NAME).py
	sed s/VERSION/$(VERSION)/g $(NAME).py > $(NAME)
	chmod 755 $(NAME)  

install :
	$(INSTALL_PROGRAM) $(NAME) $(DESTDIR)$(bindir)/$(NAME)

clean :
	$(RM) $(RUBBISH)

# this script requires that the other programs have 
# already been installed, so the "standard" test target
# is a no-op

test check :

include ../Common.mk
