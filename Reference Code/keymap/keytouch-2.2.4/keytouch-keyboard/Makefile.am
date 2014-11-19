## Process this file with automake to produce Makefile.in

SUBDIRS = src po

EXTRA_DIST = \
	autogen.sh \
	keytouch-keyboard.glade \
	keytouch-keyboard.gladep

install-data-local:
	@$(NORMAL_INSTALL)
	cp ./keytouch-keyboard $(DESTDIR)$(bindir)
	chmod o+rx $(DESTDIR)$(bindir)/keytouch-keyboard
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi

uninstall-local:
	rm $(DESTDIR)$(bindir)/keytouch-keyboard


dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

