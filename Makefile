CC=$(CROSS)gcc
CXX=$(CROSS)g++
LD=$(CROSS)ld
AR=$(CROSS)ar
PKG_CONFIG=$(CROSS)pkg-config

BASE_LIBS=
BASE_CFLAGS=-c -g -O2 -Wall -std=c++11

BASE_LIBS+= -rdynamic `$(PKG_CONFIG) --libs gtk+-3.0` -lpthread
BASE_CFLAGS+= -c `$(PKG_CONFIG) --cflags gtk+-3.0`

PROGRAMNAME=picpuz

O_FILES=picpuz.o zfuncs.o

total: ${PROGRAMNAME}

clean: 
	rm -f */*.o *.o *.P */*.P ${PROGRAMNAME}

${PROGRAMNAME}: $(O_FILES)
	$(CXX) -O -o ${PROGRAMNAME} $(O_FILES) $(BASE_LIBS)

%.o : %.cc
	$(CXX) -MD ${BASE_CFLAGS} -o $@ $<
	@mkdir -p `dirname .$(CROSS)deps/$*.P` && cp $*.d .$(CROSS)deps/$*.P; \
             sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
                 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> .$(CROSS)deps/$*.P; \
             rm -f $*.d

-include .$(CROSS)deps/*.P

SOURCE_FILES = $(O_FILES:.o=.cc)

install: $(PROGRAM) uninstall
	mkdir -p  $(DESTDIR)$(BINDIR)
	mkdir -p  $(DESTDIR)$(DATADIR)
	mkdir -p  $(DESTDIR)$(ICONDIR)
	mkdir -p  $(DESTDIR)$(LOCALESDIR)
	mkdir -p  $(DESTDIR)$(DOCDIR)
	mkdir -p  $(DESTDIR)$(MANDIR)
	mkdir -p  $(DESTDIR)$(PREFIX)/share/applications
	mkdir -p  $(DESTDIR)$(APPDATADIR)
	cp -f  $(PROGRAM) $(DESTDIR)$(BINDIR)
	cp -f -R  data/* $(DESTDIR)$(DATADIR)
	cp -f -R  icons/* $(DESTDIR)$(ICONDIR)
	cp -f -R  locales/* $(DESTDIR)$(LOCALESDIR)
	cp -f -R  doc/* $(DESTDIR)$(DOCDIR)
	gzip -f -9 $(DESTDIR)$(DOCDIR)/changelog
	# man page
	cp -f doc/$(PROGRAM).man $(PROGRAM).1
	gzip -f -9 $(PROGRAM).1
	cp $(PROGRAM).1.gz $(DESTDIR)$(MANDIR)
	rm -f $(PROGRAM).1.gz
	# menu (desktop) file
	cp -f desktop $(DESTDIR)$(MENUFILE)

uninstall:
	rm -f  $(DESTDIR)$(BINDIR)/$(PROGRAM)
	rm -f -R  $(DESTDIR)$(SHAREDIR)
	rm -f -R  $(DESTDIR)$(DOCDIR)
	rm -f  $(DESTDIR)$(MANDIR)/$(PROGRAM).1.gz
	rm -f  $(DESTDIR)$(MENUFILE)